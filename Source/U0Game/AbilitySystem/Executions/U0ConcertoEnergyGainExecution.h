// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "U0ConcertoEnergyGainExecution.generated.h"

/**
 * 
 */
UCLASS()
class U0GAME_API UU0ConcertoEnergyGainExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UU0ConcertoEnergyGainExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
