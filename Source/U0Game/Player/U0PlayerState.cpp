// Fill out your copyright notice in the Description page of Project Settings.


#include "U0PlayerState.h"
#include "U0Game/GameModes/U0ExperienceManagerComponent.h"
#include "U0Game/GameModes/U0GameModeBase.h"
#include "U0Game/Character/U0PawnData.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"
#include "U0Game/AbilitySystem/U0AbilitySet.h"
#include "U0Game/AbilitySystem/Attributes/U0HealthSet.h"
#include "U0Game/AbilitySystem/Attributes/U0CombatSet.h"
#include "U0Game/AbilitySystem/Attributes/U0HeroSkillSetBase.h"
#include "Components/GameFrameworkComponentManager.h"
#include "U0Game/Player/U0PlayerController.h"
#include "U0Game/Input/U0InputConfig.h"
#include "U0Game/CharacterManagement/U0PartyManagementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0PlayerState)

const FName AU0PlayerState::NAME_U0AbilityReady("U0AbilitiesReady");

AU0PlayerState::AU0PlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UU0AbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));

	CreateDefaultSubobject<UU0CombatSet>(TEXT("CombatSet"));
	CreateDefaultSubobject<UU0HealthSet1>(TEXT("HealthSet1"));
	CreateDefaultSubobject<UU0HealthSet2>(TEXT("HealthSet2"));
	CreateDefaultSubobject<UU0HealthSet3>(TEXT("HealthSet3"));

	CreateDefaultSubobject<UU0HeroSkillSet1>(TEXT("SkillSet1"));
	CreateDefaultSubobject<UU0HeroSkillSet2>(TEXT("SkillSet2"));
	CreateDefaultSubobject<UU0HeroSkillSet3>(TEXT("SkillSet3"));

}

void AU0PlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	// 아래의 코드는 우리가 InitAbilityActorInfo를 재호출을 통하는 이유를 설명하는 코드이다:
	{
		// 처음 InitAbilityActorInfo를 호출 당시, OwnerActor와 AvatarActor가 같은 Actor를 가르키고 있으며, 이는 PlayerState이다
		// - OwnerActor는 PlayerState가 의도하느게 맞지만, AvatarActor는 PlayerController가 소유하는 대상인 Pawn이 되어야 한다!
		// - 이를 위해 재-세팅을 해준다
		FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get();
		check(ActorInfo->OwnerActor == this);
		check(ActorInfo->OwnerActor == ActorInfo->AvatarActor);
	}
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());


	const AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);

	UU0ExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UU0ExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnU0ExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AU0PlayerState::OnExperienceLoaded(const UU0ExperienceDefinition* CurrentExperience)
{
	// PawnData Caching
	if (AU0GameModeBase* GameMode = GetWorld()->GetAuthGameMode<AU0GameModeBase>())
	{
		// AU0GameModeBase에서 GetPawnDataForController를 구현해야 됨
		// - GetPawndDataForController에서 우리는 아직 PawnData를 설정하지 않았기에, ExperienceManagerComponent의 DefaultPawnData로 설정한다
		if (const UU0PawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}


		if (UU0PartyManagementComponent* U0PMC = FindComponentByClass<UU0PartyManagementComponent>())
		{
			U0PMC->InitializePartyList();
		}

		UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_U0AbilityReady);
	}
}

void AU0PlayerState::SetPawnData(const UU0PawnData* InPawnData)
{
	check(InPawnData);

	// PawnData가 두번 설정되는 것을 원하지 않음
	check(!PawnData);

	PawnData = InPawnData;

	if (AU0PlayerController* U0PC = CastChecked<AU0PlayerController>(GetPlayerController()))
	{
		if (const UU0InputConfig* InputConfig = PawnData->InputConfig)
		{
			U0PC->InitializePlayerInput(InputConfig);
		}
	}

	// PawnData의 AbilitySets을 순회하며, ASC에 Ability를 할당(Give)한다
	// - 이 과정에서 ASC의 ActivatableAbilities에 추가된다
	for (UU0AbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
}

UAbilitySystemComponent* AU0PlayerState::GetAbilitySystemComponent() const
{
	return GetU0AbilitySystemComponent();
}

