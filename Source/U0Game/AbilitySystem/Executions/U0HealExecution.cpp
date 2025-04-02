// Fill out your copyright notice in the Description page of Project Settings.


#include "U0HealExecution.h"
#include "U0Game/AbilitySystem/Attributes/U0HealthSet.h"
#include "U0Game/AbilitySystem/Attributes/U0CombatSet.h"
#include "U0Game/Character/U0Character.h"
#include "U0Game/Player/U0PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0HealExecution)

struct FHealStatics
{
	/* AttributeSet의 어떤 Attribute을 Capture할 것인지와 어떻게 Capture할지 정의를 담고 있다 (한번 보고 오자) */
	FGameplayEffectAttributeCaptureDefinition BaseHealDef;

	FHealStatics()
	{
		BaseHealDef = FGameplayEffectAttributeCaptureDefinition(UU0CombatSet::GetBaseHealAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FHealStatics& HealStatics()
{
	// 계속 FHealStatics를 생성하는 것은 부하이니, 한번만 생성하고 재사용한다
	static FHealStatics Statics;
	return Statics;
}

UU0HealExecution::UU0HealExecution()
	: Super()
{
	// Source로 (입력값) Attribute를 캡쳐를 정의하자
	// - CombatSet::BaseHeal을 통해 Healing 값을 정의하고 최종 Execute할 때, 해당 값을 가져와서 Health에 Healing을 적용한다
	RelevantAttributesToCapture.Add(HealStatics().BaseHealDef);
}

void UU0HealExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// GameplayEffectSpec은 GameplayEffect의 핸들로 생각하면 된다
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayEffectContextHandle& EffectContext = Spec.GetEffectContext();
	int32 SlotIndex = Spec.GetLevel();

	FAggregatorEvaluateParameters EvaluateParameters;
	float BaseHeal = 0.0f;
	
	// 해당 함수 호출을 통해 U0CombatSet의 BaseHeal 값을 가져온다 (혹은 뭔가 Modifier에 누적되어 있었다면, 최종 계산 결과가 나온다)
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealDef, EvaluateParameters, BaseHeal);

	BaseHeal = HealCalculation(BaseHeal, ExecutionParams);

	// RelevantAttributesCapture을 통해 최종 계산된 BaseHeal을 0.0이하가 되지 않도록 한다 (Healing이니까)
	const float HealingDone = FMath::Max(0.0f, BaseHeal);
	if (HealingDone > 0.0f)
	{
		// GameplayEffectCalculation 이후, Modifier로서 추가한다:
		// - 해당 Modifier는 CombatSet에서 가져온 BaseHeal을 활용하여, HealthSet의 Healing에 추가해준다
		switch (SlotIndex)
		{
		case 1:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HealthSet2::GetHealing_Attribute(), EGameplayModOp::Additive, HealingDone));
			break;
		case 2:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HealthSet3::GetHealing_Attribute(), EGameplayModOp::Additive, HealingDone));
			break;
		case 0:
		default:
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UU0HealthSet1::GetHealing_Attribute(), EGameplayModOp::Additive, HealingDone));
			break;
		}
		
	}
}

float UU0HealExecution::HealCalculation(float Heal, const FGameplayEffectCustomExecutionParameters& ExecutionParams) const
{
	return Heal;
}
