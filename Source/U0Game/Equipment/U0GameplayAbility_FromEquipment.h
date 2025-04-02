// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0Game/AbilitySystem/Abilities/U0GameplayAbility.h"
#include "U0GameplayAbility_FromEquipment.generated.h"

class UU0EquipmentInstance;

/**
 * 
 */
UCLASS()
class U0GAME_API UU0GameplayAbility_FromEquipment : public UU0GameplayAbility
{
	GENERATED_BODY()
public:
	UU0EquipmentInstance* GetAssociatedEquipment() const;
};
