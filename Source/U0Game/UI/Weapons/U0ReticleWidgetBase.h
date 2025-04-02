// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "U0ReticleWidgetBase.generated.h"

/* forward declaration */
class UU0WeaponInstance;
class UU0InventoryItemInstance;

/**
 * 
 */
UCLASS(Abstract)
class U0GAME_API UU0ReticleWidgetBase : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	UU0ReticleWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void InitializeFromWeapon(UU0WeaponInstance* InWeapon);

	/*
	* WeaponInstance/InventoryInstance의 상태 추적용으로 캐싱
	*/
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UU0WeaponInstance> WeaponInstance;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UU0InventoryItemInstance> InventoryInstance;
};
