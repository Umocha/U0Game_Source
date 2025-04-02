// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "U0HUD.generated.h"

/**
 * 
 */
UCLASS()
class U0GAME_API AU0HUD : public AHUD
{
	GENERATED_BODY()
public:
	AU0HUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* GameFrameworkComponentManager의 AddReceiver를 위한 메서드들
	*/
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
