// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "U0HealExecution.generated.h"

/**
 * 
 */
UCLASS()
class U0GAME_API UU0HealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UU0HealExecution();

	/* 해당 메서드는 GameplayEffectExecutionCalculation의 Execute() BlueprintNativeEvent를 오버라이드 한다 */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	virtual float HealCalculation(float Heal, const FGameplayEffectCustomExecutionParameters& ExecutionParams) const;
};
