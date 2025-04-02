// Fill out your copyright notice in the Description page of Project Settings.


#include "U0PawnExtensionComponent.h"
#include "U0PawnData.h"
#include "Components/GameFrameworkComponentManager.h"
#include "U0Game/U0LogChannels.h"
#include "U0Game/U0GameplayTags.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0PawnExtensionComponent)

/* feature name는 component 단위니까 component를 생략 하고 pawn extension만 넣는 것을 확인할 수 있다. */
const FName UU0PawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UU0PawnExtensionComponent::UU0PawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UU0PawnExtensionComponent::SetPawnData(const UU0PawnData* InPawnData)
{
	// 데디케이트 고려X
	// Pawn에 대해 Authority가 없을 경우, SetPawnData는 진행하지 않음
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	// PawnData 업데이트
	PawnData = InPawnData;
}

void UU0PawnExtensionComponent::SetupPlayerInputComponent()
{
	// ForceUpdate로 다시 InitState 상태 변환 시작
	CheckDefaultInitialization();
}

void UU0PawnExtensionComponent::InitializeAbilitySystem(UU0AbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC && InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();
	AbilitySystemComponent = InASC;
	if (ExistingAvatar == nullptr)
	{
		AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);
	}

	if (ensure(PawnData))
	{
		InASC->AddTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}
	// check(!ExistingAvatar);
	// ASC를 업데이트하고, InitAbilityActorInfo를 Pawn과 같이 호출하여, AvatarActor를 Pawn으로 업데이트 해준다


	

	// OnAbilitySystemInitialized에 바인딩된 Delegate 호출
	OnAbilitySystemInitialized.Broadcast();
}

void UU0PawnExtensionComponent::UninitializeAbilitySystem()
{
	// 많이 간략화 했다 -> Lyra를 한번 보는게 좋겠다
	if (!AbilitySystemComponent)
	{
		return;
	}

	// OnAbilitySystemUninitialized에 바인딩된 Delegate 호출
	OnAbilitySystemUninitialized.Broadcast();

	if (ensure(PawnData))
	{
		AbilitySystemComponent->RemoveTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	AbilitySystemComponent = nullptr;
}

void UU0PawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	// OnAbilitySystemInitialized에 UObject가 바인딩되어 있지 않으면 추가 (Uniqueness)
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	// 이미 ASC가 설정되어 있으면, Delegate를 추가로 바로 호출
	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UU0PawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}

void UU0PawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

		// 올바론 Actor에 등록되었는지 확인:
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(LogU0, Error, TEXT("this component has been added to BP whose base class is not a Pawn!"));
			return;
		}
	}

	// GameFrameworkComponentManager에 InitState 사용을 위해 등록 진행
	// - 등록은 상속받았던 IGameFrameworkInitStateInterface 메서드 RegisterInitStateFeature()를 활용
	// - 해당 함수를 간단히 보기
	RegisterInitStateFeature();

	// 디버깅을 위한 함수
	UGameFrameworkComponentManager* Manager = UGameFrameworkComponentManager::GetForActor(GetOwningActor());
}

void UU0PawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// FeatureName에 NAME_None을 넣으면, Actor에 등록된 Feature Component의 InitState 상태를 관찰하겠다는 의미:
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// InitState_Spawned로 상태 변환:
	// - TryToChangeInitState는 아래와 같이 진행된다:
	//    1. CanChangeInitState로 상태 변환 가능성 유무 판단
	//	  2. HandleChangeInitState로 내부 상태 변경 (Feature Component)
	//    3. BindOnActorInitStateChanged로 Bind된 Delegate를 조건에 맞게 호출해 줌
	//      - U0PawnExtensionComponent의 경우, 모든 Actor의 Feature 상태 변화에 대해 OnActorInitStateChanged()가 호출됨
	ensure(TryToChangeInitState(U0GameplayTags::InitState_Spawned));

	// 해당 함수는 우리가 오버라이딩 한다: 
	// - 이 함수를 ForceUpdateInitState와 같은 느낌으로 이해해주면 된다
	// - 현재 강제 업데이트 진행 (물론 CanChangeInitState와 HandleChangeInitState를 진행해준다)
	CheckDefaultInitialization();
}

void UU0PawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 앞서, OnRegister의 RegisterInitStateFeature()의 쌍을 맞추어주자
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UU0PawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if(Params.FeatureName != NAME_ActorFeatureName)
	{
		// U0PawnExtensionComponent는 다른 Feature Component드르이 상태가 DateAvailable를 관찰하여, Sync를 맞추는 구간이 있었다 (CanChangeInitState)
		// - 이를 가능케하기 위해, OnActorInitStateChanged에서는 DataAvailable에 대해 지속적으로 CheckDefaultInitialization을 호출하여, 상태를 체크한다
		if (Params.FeatureState == U0GameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UU0PawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	// Spawned 초기화
	if (!CurrentState.IsValid() && DesiredState == U0GameplayTags::InitState_Spawned)
	{
		// Pawn이 잘 세팅만 되어있다면 바로 Spawned로 넘어감!
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	if (CurrentState == U0GameplayTags::InitState_Spawned && DesiredState == U0GameplayTags::InitState_DataAvailable)
	{
		// 아마 PawnData를 누군가 설정하는 모양이다
		if (!PawnData)
		{
			return false;
		}

		// LocallyControlled인 Pawn이 Controller가 없으면 에러!
		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		if (bHasAuthority || bIsLocallyControlled)
		{
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == U0GameplayTags::InitState_DataAvailable && DesiredState == U0GameplayTags::InitState_DataInitialized)
	{
		// Actor에 바인드된 모든 Feature들이 DataAvailable 상태일 때, DataInitialized로 넘어감:
		// - HaveAllFeaturesReachedInitState 확인
		return Manager->HaveAllFeaturesReachedInitState(Pawn, U0GameplayTags::InitState_DataAvailable);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == U0GameplayTags::InitState_DataInitialized && DesiredState == U0GameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	// 위의 linear(선형적) transition이 아니면 false!
	return false;
}

void UU0PawnExtensionComponent::CheckDefaultInitialization()
{
	// PawnExtensionComponent는 Feature Component들의 초기화를 관장하는 Component이다:
	// - 따라서, Actor에 바인딩된 Feature Component들에 대해 CheckDefaultInitialization을 호출해주도록 한다 (ForceUpdate 느낌)
	// - 이 메서드를 IGameFrameworkInitStateInterface가 제공하는데, CheckDefaultInitializationForImplementers이다:
	CheckDefaultInitializationForImplementers();

	// 사용자 정의 InitState를 직접 넘겨줘야 한다... (사실 이건 좀 알아서 할 수 있을거 같은데 굳이)
	static const TArray<FGameplayTag> StateChain = { U0GameplayTags::InitState_Spawned, U0GameplayTags::InitState_DataAvailable, U0GameplayTags::InitState_DataInitialized, U0GameplayTags::InitState_GameplayReady };

	// CanChangeInitState()와 HandleChangeInitState() 그리고 ChangeFeatureInitState 호출을 통한 OnActorInitStateChanged Delegate 호출까지 진행해준다:
	// 아래의 코드를 간단히 보자:
	// - 해당 코드까지 보면, 우리는 간단히 짐작할 수 있다.
	//    - 계속 상태를 가능할때까지 한번 업데이트 한다
	//    - InitState에 대한 변화는 Linear(선형적)임을! -> 나중에 Diagram으로 정리하면서 보자
	//      - 업데이트가 멈추면 누군가 시작해줘야 함을 의미! (chaining)
	ContinueInitStateChain(StateChain);
}
