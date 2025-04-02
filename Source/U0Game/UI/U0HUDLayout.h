// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0ActivatableWidget.h"
#include "U0HUDLayout.generated.h"

/**
 * PrimaryGameLayout의 Layer에 연동할 HUD Layout (CommonActivatableWidget)
 */
UCLASS()
class U0GAME_API UU0HUDLayout : public UU0ActivatableWidget
{
	GENERATED_BODY()
public:
	UU0HUDLayout(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
