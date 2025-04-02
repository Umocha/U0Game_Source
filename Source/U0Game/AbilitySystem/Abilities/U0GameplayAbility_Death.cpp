// Copyright Epic Games, Inc. All Rights Reserved.

#include "U0GameplayAbility_Death.h"

#include "U0GameplayAbility.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"
#include "U0Game/Character/U0HealthComponent.h"
#include "U0Game/U0GameplayTags.h"
#include "U0Game/U0LogChannels.h"
#include "Trace/Trace.inl"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0GameplayAbility_Death)

UU0GameplayAbility_Death::UU0GameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	bAutoStartDeath = true;

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = U0GameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void UU0GameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	check(ActorInfo);

	UU0AbilitySystemComponent* U0ASC = CastChecked<UU0AbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

	FGameplayTagContainer AbilityTypesToIgnore;
	AbilityTypesToIgnore.AddTag(U0GameplayTags::Ability_Behavior_SurvivesDeath);

	// Cancel all abilities and block others from starting.
	U0ASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

	SetCanBeCanceled(false);

	//if (!ChangeActivationGroup(EU0AbilityActivationGroup::Exclusive_Blocking))
	//{
	//	UE_LOG(LogU0AbilitySystem, Error, TEXT("UU0GameplayAbility_Death::ActivateAbility: Ability [%s] failed to change activation group to blocking."), *GetName());
	//}

	if (bAutoStartDeath)
	{
		StartDeath();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UU0GameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	check(ActorInfo);

	// Always try to finish the death when the ability ends in case the ability doesn't.
	// This won't do anything if the death hasn't been started.
	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UU0GameplayAbility_Death::StartDeath()
{
	if (UU0HealthComponent* HealthComponent = UU0HealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EU0DeathState::NotDead)
		{
			HealthComponent->StartDeath();
		}
	}
}

void UU0GameplayAbility_Death::FinishDeath()
{
	if (UU0HealthComponent* HealthComponent = UU0HealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
	{
		if (HealthComponent->GetDeathState() == EU0DeathState::DeathStarted)
		{
			HealthComponent->FinishDeath();
		}
	}
}

