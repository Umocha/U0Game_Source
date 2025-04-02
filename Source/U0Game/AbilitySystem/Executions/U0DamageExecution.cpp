// Fill out your copyright notice in the Description page of Project Settings.


#include "U0DamageExecution.h"
#include "U0Game/AbilitySystem/Attributes/U0HealthSet.h"
#include "U0Game/AbilitySystem/Attributes/U0CombatSet.h"
#include "U0Game/Character/U0Character.h"
#include "U0Game/U0GameplayTags.h"
#include "U0Game/AbilitySystem/U0GameplayEffectContext.h"


struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

	FDamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UU0CombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UU0DamageExecution::UU0DamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
}

void UU0DamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE



	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayEffectContextHandle& EffectContext = Spec.GetEffectContext();
	FU0GameplayEffectContext* TypedContext = FU0GameplayEffectContext::ExtractEffectContext(EffectContext);
	check(TypedContext);

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();


	int32 SlotIndex = INDEX_NONE;
	if (UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent())
	{
		if (TargetTags->HasTag(U0GameplayTags::GameplayEvent_Dash_Just))
		{
			return;
		}

		if (AU0Character* U0Character = Cast<AU0Character>(TargetAbilitySystemComponent->GetAvatarActor_Direct()))
		{
			SlotIndex = U0Character->GetSlotIndex();
			if (Spec.GetLevel() != SlotIndex)
			{
				return;
			}
		}
	}

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);

	const float DamageDone = FMath::Max(BaseDamage, 0.0f);
	TypedContext->FloatValue = DamageDone;

	if (DamageDone > 0.0f)
	{
		// Apply a damage modifier, this gets turned into - health on the target
		switch (SlotIndex)
		{
		case 1:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HealthSet2::GetDamage_Attribute(), EGameplayModOp::Additive, DamageDone));
			break;
		case 2:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HealthSet3::GetDamage_Attribute(), EGameplayModOp::Additive, DamageDone));
			break;
		case 0:
		default:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HealthSet1::GetDamage_Attribute(), EGameplayModOp::Additive, DamageDone));
			break;
		}

	}

#endif // WITH_SERVER_CODE
}