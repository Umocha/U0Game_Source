// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0InventoryItemDefinition.h"
#include "U0InventoryFragment_EquippableItem.generated.h"

class UU0EquipmentDefinition;

/**
 * 
 */
UCLASS()
class U0GAME_API UU0InventoryFragment_EquippableItem : public UU0InventoryItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "U0")
	TSubclassOf<UU0EquipmentDefinition> EquipmentDefinition;
};
