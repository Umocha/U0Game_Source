// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0WeaponInstance.h"
#include "U0RangedWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class U0GAME_API UU0RangedWeaponInstance : public UU0WeaponInstance
{
	GENERATED_BODY()
public:

	/* 유효 사거리 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponConfig", meta = (ForceUnits = cm))
	float MaxDamageRange = 25000.0f;

	/* 총탄 범위 (Sphere Trace Sweep) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponConfig", meta = (ForceUnits = cm))
	float BulletTraceWeaponRadius = 0.0f;
};
