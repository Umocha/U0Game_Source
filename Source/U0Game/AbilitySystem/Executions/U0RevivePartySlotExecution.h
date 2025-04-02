// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "U0RevivePartySlotExecution.generated.h"

/**
 * 
 */
UCLASS()
class U0GAME_API UU0RevivePartySlotExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UU0RevivePartySlotExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
