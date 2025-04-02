// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Array.h"
#include "Templates/SubclassOf.h"
#include "U0Game/Inventory/U0InventoryItemDefinition.h"
#include "U0InventoryFragment_ReticleConfig.generated.h"

/* forward declaration */
class UU0ReticleWidgetBase;

/**
 * 
 */
UCLASS()
class U0GAME_API UU0InventoryFragment_ReticleConfig : public UU0InventoryItemFragment
{
	GENERATED_BODY()
public:
	/* 무기에 결합된 ReticleWidget 정보를 가지고 있는 Fragment */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reticle")
	TArray<TSubclassOf<UU0ReticleWidgetBase>> ReticleWidgets;
};
