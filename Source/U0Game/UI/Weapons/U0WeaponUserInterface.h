// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "U0WeaponUserInterface.generated.h"

/* forward declaration */
class UU0WeaponInstance;

/**
 * 
 */
UCLASS()
class U0GAME_API UU0WeaponUserInterface : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UU0WeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Weapon 변경에 따른 BP Event */
	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponChanged(UU0WeaponInstance* OldWeapon, UU0WeaponInstance* NewWeapon);

	/*
	* UUserWidget's interface
	*/
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/* 현재 장착된 WeaponInstance를 추적한다 (NativeTice을 활용하여 주기적으로 업데이트) */
	UPROPERTY(Transient)
	TObjectPtr<UU0WeaponInstance> CurrentInstance;
};
