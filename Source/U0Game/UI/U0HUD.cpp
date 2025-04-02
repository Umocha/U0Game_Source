// Fill out your copyright notice in the Description page of Project Settings.


#include "U0HUD.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0HUD)

AU0HUD::AU0HUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AU0HUD::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	// U0HUD를 Receiver로 Actor를 추가하자
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AU0HUD::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void AU0HUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}
