// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "U0UltimateEnergyExecution.generated.h"

/**
 * 
 */
UCLASS()
class U0GAME_API UU0UltimateEnergyExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UU0UltimateEnergyExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
