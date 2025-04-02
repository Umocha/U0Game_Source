// Fill out your copyright notice in the Description page of Project Settings.


#include "U0GameModeBase.h"
#include "U0GameState.h"
#include "U0ExperienceManagerComponent.h"
#include "U0ExperienceDefinition.h"
#include "U0Game/Player/U0PlayerController.h"
#include "U0Game/Player/U0PlayerState.h"
#include "U0Game/Character/U0Character.h"
#include "U0Game/Character/U0PawnData.h"
#include "U0Game/UI/U0HUD.h"
#include "U0Game/U0LogChannels.h"
#include "U0Game/Character/U0PawnExtensionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "U0WorldSettings.h"
#include "U0Game/System/U0AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0GameModeBase)

AU0GameModeBase::AU0GameModeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AU0GameState::StaticClass();
	PlayerControllerClass = AU0PlayerController::StaticClass();
	PlayerStateClass = AU0PlayerState::StaticClass();
	DefaultPawnClass = AU0Character::StaticClass();
	HUDClass = AU0HUD::StaticClass();
}

void AU0GameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 아직 GameInstance를 통한 초기화 작업이 진행되므로, 현 프레임에서는 Lyra의 Concept인 Experience 처리를 진행할 수 없다.
	// - 이를 처리하기 위해, 한 프레임 뒤에 이벤트를 받아 처리를 이어서 진행한다.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void AU0GameModeBase::InitGameState()
{
	Super::InitGameState();

	// Experience 비동기 로딩을 위한 Delegate를 준비한다.
	UU0ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UU0ExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	// OnExperienceLoaded 등록
	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnU0ExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* AU0GameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// GetPawnDataForController를 활용하여, PawnData로 부터 PawnClass를 유도하자
	if (const UU0PawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AU0GameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

APawn* AU0GameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			 // FindPawnExtensionComponent 구현
			if (UU0PawnExtensionComponent* PawnExtComp = UU0PawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UU0PawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
			}
			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
	}
	return nullptr;
}

void AU0GameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	// - dedicate server인지 개발자 옵션 설정인지, epic game에서 제공하는 match서비스 등등
	// - 전부 검사 후 선택해서 experience를 로딩해줌

	// 해당 함수에서는 우리가 로딩할 Experience에 대해 PrimaryAssetId를 생성하여, OnMatchAssignmentGiven으로 넘겨준다

	FPrimaryAssetId ExperienceId;

	// precedence order (highest wins)
	// - matchmaking assignment (if present)
	// - default experience

	UWorld* World = GetWorld();

	// URL과 함께 ExtraArgs로 넘겼던 정보는 OptionsString에 저장되어 있다
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		// Experience의 Value를 가져와서, PrimaryAssetId를 생성해준다: 이때, U0ExperienceDefinition의 Class 이름을 사용한다
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UU0ExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
	}

	// see if the world settings has a default experience
	if (!ExperienceId.IsValid())
	{
		if (AU0WorldSettings* TypedWorldSettings = Cast<AU0WorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
		}
	}

	// fall back to the default experience
	// 일단 기본 옵션으로 default하게 B_U0DefaultExperience로 설정해놓자
	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("U0ExperienceDefinition"), FName("B_U0DefaultExperience"));
	}

	// 아직 HandleMatchAssignmentIfNotExpectingOne()과 OnMatchAssignmentGiven()은 아직 직관적으로 이름이 와닿지 않는다고 생각함
	// - 후일, 어느정도Lyra가 구현되면, 해당 함수의 명을 더 이해할 수 있을 것으로 예상함
	OnMatchAssignmentGiven(ExperienceId);
}

void AU0GameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
	// 해당 함수는 ExperienceManagerComponent을 활용하여 Experience을 로딩하기 위해, ExperienceManagerComponent의 ServerSetCurrentExperience를 호출한다

	check(ExperienceId.IsValid());

	UU0ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UU0ExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	ExperienceManagerComponent->ServerSetCurrentExperience(ExperienceId);
}

bool AU0GameModeBase::IsExperienceLoaded() const
{
	check(GameState);
	UU0ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UU0ExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	return ExperienceManagerComponent->IsExperienceLoaded();
}

void AU0GameModeBase::OnExperienceLoaded(const UU0ExperienceDefinition* CurrentExperience)
{
	// PlayerController를 순회 하며
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		// PlayerController가 Pawn을 Posseess하지 않았다면, RestartPlayer를 통해 Pawn을 다시 Spawn한다
		// - 한번 OnPossess를 보도록 하자:
		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

const UU0PawnData* AU0GameModeBase::GetPawnDataForController(const AController* InController) const
{
	// 게임 도중 PawnData가 오버라이드 되었을 경우, PawnData는 PlayerState에서 가져오게 됨
	if (InController)
	{
		if (const AU0PlayerState* U0PS = InController->GetPlayerState<AU0PlayerState>())
		{
			// GetPawnData 구현
			if (const UU0PawnData* PawnData = U0PS->GetPawnData<UU0PawnData>())
			{
				return PawnData;
			}
		}
	}

	// fall back to the default for the current experience
	// 아직 PlayerState에 PawnData가 설정되어 있지 않은 경우, ExperienceManagerComponent의 CurrentExperience로 부터 가져와서 설정
	check(GameState);
	UU0ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UU0ExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		// GetExperienceChecked 구현
		const UU0ExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}

		//return UU0AssetManager::Get().GetDefaultPawnData();
	}

	// 어떠한 케이스에도 핸들링이 안되었으면 nullptr
	return nullptr;
}
