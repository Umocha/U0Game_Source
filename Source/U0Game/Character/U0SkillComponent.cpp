// Fill out your copyright notice in the Description page of Project Settings.


#include "U0SkillComponent.h"
#include "U0Game/U0LogChannels.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"
#include "U0Game/AbilitySystem/Attributes/U0HeroSkillSetBase.h"
#include "GameplayEffectExtension.h"
#include "U0CharacterData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0SkillComponent)

UU0SkillComponent::UU0SkillComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// HealthComponent는 PlayerState의 HealthSet과 Character(Pawn)가 Brige 역할이라고 생각하면 된다:
	// - 따로 로직이 업데이트 될 필요가 없는 이벤트 기반으로 동작하는 컴포넌트로 이해하면 된다
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	// InitializeWithAbilitySystem으로 ASC가 초기화되기 전까지 HealthSet과 ASC는 null이다:
	AbilitySystemComponent = nullptr;
	SkillSet = nullptr;
}

UU0SkillComponent* UU0SkillComponent::FindSkillComponent(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	UU0SkillComponent* HealthComponent = Actor->FindComponentByClass<UU0SkillComponent>();
	return HealthComponent;
}

float UU0SkillComponent::GetConcertoEnergy() const
{
	return (SkillSet ? SkillSet->GetConcertoEnergy() : 0.0f);
}

float UU0SkillComponent::GetMaxConcertoEnergy() const
{
	return (SkillSet ? SkillSet->GetMaxConcertoEnergy() : 0.0f);
}

float UU0SkillComponent::GetUltimateEnergy() const
{
	return (SkillSet ? SkillSet->GetUltimateEnergy() : 0.0f);
}

float UU0SkillComponent::GetMaxUltimateEnergy() const
{
	return (SkillSet ? SkillSet->GetMaxUltimateEnergy() : 0.0f);
}

float UU0SkillComponent::GetConcertoEnergyNormalized() const
{
	if (SkillSet)
	{
		const float Concerto = SkillSet->GetConcertoEnergy();
		const float MaxConcerto = SkillSet->GetMaxConcertoEnergy();
		return ((MaxConcerto > 0.0f) ? (Concerto / MaxConcerto) : 0.0f);
	}
	return 0.0f;
}

float UU0SkillComponent::GetUltimateEnergyNormalized() const
{
	if (SkillSet)
	{
		const float Ultimate = SkillSet->GetUltimateEnergy();
		const float MaxUltimate = SkillSet->GetMaxUltimateEnergy();
		return ((MaxUltimate > 0.0f) ? (Ultimate / MaxUltimate) : 0.0f);
	}
	return 0.0f;
}

void UU0SkillComponent::InitializeWithAbilitySystem(UU0AbilitySystemComponent* InASC, const FCharacterAttributeData& CharacterAttributeData, int32 SlotIndex)
{
	// AActor는 U0Character를 상속받고 있는 클래스일 것이다
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogU0, Error, TEXT("U0SkillComponent: Skill component for owner [%s] has already been initialized with an ability system"), *GetNameSafe(Owner));
		return;
	}

	// ASC 캐싱
	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogU0, Error, TEXT("U0SkillComponent: Cannot initialize skill component for owner [%s] with NULL ability system"), *GetNameSafe(Owner));
		return;
	}

	// AbilitySystemComponent::GetSet은 SpawnedAttributes에서 가져온다:
	// - 근데 PlayerState에서 Subobject로 생성하고 따로 ASC에 등록한적이 없는데 어떻게 등록되어있을까?
	//   - AbilitySystemComponent::InitializeComponent()에서 GetObjectWithOuter로 SpawnedAttributes에 추가된다:
	//   - 잘 생각해보자 HealthSet은 PlayerState에 Subobject로 있고, ASC 또한 PlayerState에 있다:
	//     -> 이는 ASC에서 GetObjectsWithOuter로 HealthSet이 접근가능하다 볼 수 있다
	// - 한번 AbilitySystemComponent::InitializeComponent()을 보자

	SkillSet = AbilitySystemComponent->GetSkillSet(SlotIndex);
	
	

	if (!SkillSet)
	{
		UE_LOG(LogU0, Error, TEXT("U0SkillComponent: Cannot initialize skill component for owner [%s] with NULL health set on the ability system"), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	// HealthSet의 HealthAttribute의 업데이트가 일어날 때 마다 호출할 콜백 멤버메서드 HandleHealthChaged를 등록하자:
	SkillSet->OnConcertoEnergyChanged.AddUObject(this, &ThisClass::HandleConcertoEnergyChanged);
	SkillSet->OnMaxConcertoEnergyChanged.AddUObject(this, &ThisClass::HandleMaxConcertoEnergyChanged);
	SkillSet->OnUltimateEnergyChanged.AddUObject(this, &ThisClass::HandleUltimateEnergyChanged);
	SkillSet->OnMaxUltimateEnergyChanged.AddUObject(this, &ThisClass::HandleMaxUltimateEnergyChanged);

	// TODO : U0 - PawnData에 저장된 값을 입력해주자( Health, MaxHealth )
	AbilitySystemComponent->SetNumericAttributeBase(SkillSet->GetMaxConcertoEnergyAttribute(), CharacterAttributeData.MaxConcertoEnergy);
	AbilitySystemComponent->SetNumericAttributeBase(SkillSet->GetMaxUltimateEnergyAttribute(), CharacterAttributeData.MaxUltimateEnergy);
	AbilitySystemComponent->SetNumericAttributeBase(SkillSet->GetConcertoEnergyAttribute(), CharacterAttributeData.ConcertoEnergy);
	AbilitySystemComponent->SetNumericAttributeBase(SkillSet->GetUltimateEnergyAttribute(), CharacterAttributeData.UltimateEnergy);

	// 초기화 한번 해줬으니까 Broadcast 해주자
	OnConcertoEnergyChanged.Broadcast(this, 0, SkillSet->GetConcertoEnergy(), nullptr);
	OnUltimateEnergyChanged.Broadcast(this, 0, SkillSet->GetUltimateEnergy(), nullptr);
}

void UU0SkillComponent::UninitializeWithAbilitySystem()
{

	if (SkillSet)
	{
		SkillSet->OnConcertoEnergyChanged.RemoveAll(this);
		SkillSet->OnMaxConcertoEnergyChanged.RemoveAll(this);
		SkillSet->OnUltimateEnergyChanged.RemoveAll(this);
		SkillSet->OnMaxUltimateEnergyChanged.RemoveAll(this);
	}

	AbilitySystemComponent = nullptr;
	SkillSet = nullptr;
}

void UU0SkillComponent::HandleConcertoEnergyChanged(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue)
{
	OnConcertoEnergyChanged.Broadcast(this, OldValue, NewValue, Instigator);
}

void UU0SkillComponent::HandleMaxConcertoEnergyChanged(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue)
{
	OnMaxConcertoEnergyChanged.Broadcast(this, OldValue, NewValue, Instigator);
}

void UU0SkillComponent::HandleUltimateEnergyChanged(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue)
{
	OnUltimateEnergyChanged.Broadcast(this, OldValue, NewValue, Instigator);
}

void UU0SkillComponent::HandleMaxUltimateEnergyChanged(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue)
{
	OnMaxUltimateEnergyChanged.Broadcast(this, OldValue, NewValue, Instigator);
}
