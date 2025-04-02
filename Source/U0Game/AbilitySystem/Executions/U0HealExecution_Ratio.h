// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0HealExecution.h"
#include "U0HealExecution_Ratio.generated.h"

/**
 * 
 */
UCLASS()
class U0GAME_API UU0HealExecution_Ratio : public UU0HealExecution
{
	GENERATED_BODY()
public:
	virtual float HealCalculation(float Heal, const FGameplayEffectCustomExecutionParameters& ExecutionParams) const;
};
