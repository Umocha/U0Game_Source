// Fill out your copyright notice in the Description page of Project Settings.


#include "U0UltimateEnergyExecution.h"
#include "U0Game/Character/U0Character.h"
#include "U0Game/AbilitySystem/Attributes/U0HeroSkillSetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0UltimateEnergyExecution)

struct FUltiEnergyStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseEnergyDef;

	FUltiEnergyStatics()
	{
		BaseEnergyDef = FGameplayEffectAttributeCaptureDefinition(UU0HeroSkillSet1::GetUltimateEnergyRegen_Attribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FUltiEnergyStatics& UltiEnergyStatics()
{
	static FUltiEnergyStatics Statics;
	return Statics;
}

UU0UltimateEnergyExecution::UU0UltimateEnergyExecution()
	: Super()
{
	RelevantAttributesToCapture.Add(UltiEnergyStatics().BaseEnergyDef);
}

void UU0UltimateEnergyExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayEffectContextHandle& EffectContext = Spec.GetEffectContext();
	int32 SlotIndex = Spec.GetLevel();

	float GainEnergy = 0.0f;
	
	if (Spec.Def->Executions.IsValidIndex(0))
	{
		for (const FGameplayEffectExecutionScopedModifierInfo& Modifier : Spec.Def->Executions[0].CalculationModifiers)
		{
			if (Modifier.ModifierMagnitude.GetMagnitudeCalculationType() == EGameplayEffectMagnitudeCalculation::ScalableFloat)
			{
				Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(SlotIndex, GainEnergy);
			}
		}
	}

	const float RegenDone = GainEnergy;
	if (RegenDone != 0.0f)
	{
		switch (SlotIndex)
		{
		case 1:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HeroSkillSet2::GetUltimateEnergyRegen_Attribute(), EGameplayModOp::Additive, RegenDone));
			break;
		case 2:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HeroSkillSet3::GetUltimateEnergyRegen_Attribute(), EGameplayModOp::Additive, RegenDone));
			break;
		case 0:
		default:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HeroSkillSet1::GetUltimateEnergyRegen_Attribute(), EGameplayModOp::Additive, RegenDone));
			break;
		}

	}
}
