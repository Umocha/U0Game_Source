// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "U0GameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class U0GAME_API UU0GameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()
public:
	UU0GameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UU0GameplayCueManager* Get();

	void RefreshGameplayCuePrimaryAsset();
};
