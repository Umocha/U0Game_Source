// Fill out your copyright notice in the Description page of Project Settings.


#include "U0ConcertoEnergyGainExecution.h"
#include "U0Game/Character/U0Character.h"
#include "U0Game/AbilitySystem/Attributes/U0HeroSkillSetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0ConcertoEnergyGainExecution)

struct FEnergyStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseEnergyDef;

	FEnergyStatics()
	{
		BaseEnergyDef = FGameplayEffectAttributeCaptureDefinition(UU0HeroSkillSet1::GetConcertoEnergyRegen_Attribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FEnergyStatics& EnergyStatics()
{
	static FEnergyStatics Statics;
	return Statics;
}

UU0ConcertoEnergyGainExecution::UU0ConcertoEnergyGainExecution()
	: Super()
{
	RelevantAttributesToCapture.Add(EnergyStatics().BaseEnergyDef);
}

void UU0ConcertoEnergyGainExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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

	const float RegenDone = FMath::Max(0.0f, GainEnergy);
	if (RegenDone > 0.0f)
	{
		// GameplayEffectCalculation 이후, Modifier로서 추가한다:
		// - 해당 Modifier는 CombatSet에서 가져온 BaseHeal을 활용하여, HealthSet의 Healing에 추가해준다
		switch (SlotIndex)
		{
		case 1:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HeroSkillSet2::GetConcertoEnergyRegen_Attribute(), EGameplayModOp::Additive, RegenDone));
			break;
		case 2:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HeroSkillSet3::GetConcertoEnergyRegen_Attribute(), EGameplayModOp::Additive, RegenDone));
			break;
		case 0:
		default:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HeroSkillSet1::GetConcertoEnergyRegen_Attribute(), EGameplayModOp::Additive, RegenDone));
			break;
		}

	}
}
