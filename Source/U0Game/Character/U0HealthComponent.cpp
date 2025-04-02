// Fill out your copyright notice in the Description page of Project Settings.


#include "U0HealthComponent.h"
#include "U0Game/U0LogChannels.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"
#include "U0Game/AbilitySystem/Attributes/U0HealthSet.h"
#include "GameplayEffectExtension.h"
#include "U0PawnData.h"
#include "U0Game/U0GameplayTags.h"
#include "U0Game/Messages/U0VerbMessage.h"
#include "U0Game/Messages/U0VerbMessageHelpers.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "U0Game/Character/U0Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0HealthComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_U0_Elimination_Message, "U0.Elimination.Message");

UU0HealthComponent::UU0HealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// HealthComponent는 PlayerState의 HealthSet과 Character(Pawn)가 Brige 역할이라고 생각하면 된다:
	// - 따로 로직이 업데이트 될 필요가 없는 이벤트 기반으로 동작하는 컴포넌트로 이해하면 된다
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	// InitializeWithAbilitySystem으로 ASC가 초기화되기 전까지 HealthSet과 ASC는 null이다:
	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
}

UU0HealthComponent* UU0HealthComponent::FindHealthComponent(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	UU0HealthComponent* HealthComponent = Actor->FindComponentByClass<UU0HealthComponent>();
	return HealthComponent;
}

float UU0HealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float UU0HealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float UU0HealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();
		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}
	return 0.0f;
}

void UU0HealthComponent::InitializeWithAbilitySystem(UU0AbilitySystemComponent* InASC, const FPawnAttributeData& AttributeData, int32 SlotIndex)
{
	// AActor는 U0Character를 상속받고 있는 클래스일 것이다
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogU0, Error, TEXT("U0HealthComponent: Health component for owner [%s] has already been initialized with an ability system"), *GetNameSafe(Owner));
		return;
	}

	// ASC 캐싱
	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogU0, Error, TEXT("U0HealthComponent: Cannot initialize health component for owner [%s] with NULL ability system"), *GetNameSafe(Owner));
		return;
	}

	// AbilitySystemComponent::GetSet은 SpawnedAttributes에서 가져온다:
	// - 근데 PlayerState에서 Subobject로 생성하고 따로 ASC에 등록한적이 없는데 어떻게 등록되어있을까?
	//   - AbilitySystemComponent::InitializeComponent()에서 GetObjectWithOuter로 SpawnedAttributes에 추가된다:
	//   - 잘 생각해보자 HealthSet은 PlayerState에 Subobject로 있고, ASC 또한 PlayerState에 있다:
	//     -> 이는 ASC에서 GetObjectsWithOuter로 HealthSet이 접근가능하다 볼 수 있다
	// - 한번 AbilitySystemComponent::InitializeComponent()을 보자

	HealthSet = AbilitySystemComponent->GetHealthSet(SlotIndex);
	
	

	if (!HealthSet)
	{
		UE_LOG(LogU0, Error, TEXT("U0HealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system"), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	// HealthSet의 HealthAttribute의 업데이트가 일어날 때 마다 호출할 콜백 멤버메서드 HandleHealthChaged를 등록하자:
	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	// TODO : U0 - PawnData에 저장된 값을 입력해주자( Health, MaxHealth )
	AbilitySystemComponent->SetNumericAttributeBase(HealthSet->GetMaxHealthAttribute(), AttributeData.MaxHealth);
	AbilitySystemComponent->SetNumericAttributeBase(HealthSet->GetHealthAttribute(), AttributeData.Health);
	if (AttributeData.Health < 0)
	{
		AbilitySystemComponent->SetNumericAttributeBase(HealthSet->GetHealthAttribute(), HealthSet->GetMaxHealth());
	}

	// 초기화 한번 해줬으니까 Broadcast 해주자
	OnHealthChanged.Broadcast(this, 0, HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

void UU0HealthComponent::UninitializeWithAbilitySystem()
{

	if (HealthSet)
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
}

void UU0HealthComponent::StartDeath()
{
	if (DeathState != EU0DeathState::NotDead)
	{
		return;
	}

	DeathState = EU0DeathState::DeathStarted;

	AActor* Owner = GetOwner();
	check(Owner);

	AU0Character* U0Character = Cast<AU0Character>(Owner);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(U0GameplayTags::Status_Death_Dying, 1);
	}

	OnDeathStarted.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UU0HealthComponent::FinishDeath()
{
	if (DeathState != EU0DeathState::DeathStarted)
	{
		return;
	}

	DeathState = EU0DeathState::DeathFinished;



	AActor* Owner = GetOwner();
	check(Owner);

	AU0Character* U0Character = Cast<AU0Character>(Owner);
	if (AbilitySystemComponent) 
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(U0GameplayTags::Status_Death_Dying);
		if (U0Character == nullptr)
		{
			AbilitySystemComponent->SetLooseGameplayTagCount(U0GameplayTags::Status_Death_Dead, 1);
		}
	}


	OnDeathFinished.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UU0HealthComponent::Revive()
{
	if (DeathState != EU0DeathState::DeathFinished)
	{
		return;
	}

	DeathState = EU0DeathState::NotDead;

	AActor* Owner = GetOwner();
	check(Owner);

	OnRevived.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UU0HealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UU0HealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UU0HealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
	if (AbilitySystemComponent && DamageEffectSpec)
	{
		// "GameplayEvent.Death" Owner에게 Send. GA(Death)를 trigger.
		{
			FGameplayEventData Payload;
			Payload.EventTag = U0GameplayTags::GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec->Def;
			Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		// 다른 systems에 observe 할 수 있게 Message send.
		{
			FU0VerbMessage Message;
			Message.Verb = TAG_U0_Elimination_Message;
			Message.Instigator = DamageInstigator;
			Message.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Message.Target = UU0VerbMessageHelpers::GetPlayerStateFromObject(AbilitySystemComponent->GetAvatarActor());
			Message.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}
	}
#endif // WITH_SERVER_CODE
}
