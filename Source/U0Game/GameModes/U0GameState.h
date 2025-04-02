// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameState.h"
#include "U0GameState.generated.h"

class UU0ExperienceManagerComponent;
/**
 * 
 */
UCLASS()
class U0GAME_API AU0GameState : public AModularGameStateBase
{
	GENERATED_BODY()
	
public:
	AU0GameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY()
	TObjectPtr<UU0ExperienceManagerComponent> ExperienceManagerComponent;
};
