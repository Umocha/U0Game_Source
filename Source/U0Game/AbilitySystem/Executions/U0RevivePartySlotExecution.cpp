// Fill out your copyright notice in the Description page of Project Settings.


#include "U0RevivePartySlotExecution.h"
#include "U0Game/Player/U0PlayerState.h"
#include "U0Game/CharacterManagement/U0PartyManagementComponent.h"
#include "U0Game/AbilitySystem/Attributes/U0HealthSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0RevivePartySlotExecution)


UU0RevivePartySlotExecution::UU0RevivePartySlotExecution()
	: Super()
{
}

void UU0RevivePartySlotExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	int32 SlotIndex = Spec.GetLevel();

	if (UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent())
	{
		if (AActor* Owner = TargetAbilitySystemComponent->GetOwnerActor())
		{
			if (AU0PlayerState* U0PS = Cast<AU0PlayerState>(Owner))
			{
				if (UU0PartyManagementComponent* U0PMC = U0PS->FindComponentByClass<UU0PartyManagementComponent>())
				{
					if (U0PMC->ReviveSlot(SlotIndex))
					{
						switch (SlotIndex)
						{
						case 1:
							OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HealthSet2::GetHealth_Attribute(), EGameplayModOp::Override, 1));
							break;
						case 2:
							OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HealthSet3::GetHealth_Attribute(), EGameplayModOp::Override, 1));
							break;
						case 0:
						default:
							OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HealthSet1::GetHealth_Attribute(), EGameplayModOp::Override, 1));
							break;
						}
					}
				}
			}
		}
	}
}
