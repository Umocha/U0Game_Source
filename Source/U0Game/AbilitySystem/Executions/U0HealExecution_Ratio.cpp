// Fill out your copyright notice in the Description page of Project Settings.


#include "U0HealExecution_Ratio.h"
#include "U0Game/AbilitySystem/Attributes/U0HealthSet.h"
#include "U0Game/Character/U0Character.h"
#include "U0Game/Character/U0HealthComponent.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"

float UU0HealExecution_Ratio::HealCalculation(float Heal, const FGameplayEffectCustomExecutionParameters& ExecutionParams) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	int32 SlotIndex = Spec.GetLevel();

	float MaxHealth = 1.0f;

	if (UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent())
	{
		if (UU0AbilitySystemComponent* U0ASC = Cast<UU0AbilitySystemComponent>(TargetAbilitySystemComponent))
		{
			MaxHealth = U0ASC->GetHealthSet(SlotIndex)->GetMaxHealth();
		}
	}


	return Heal * MaxHealth;
}
