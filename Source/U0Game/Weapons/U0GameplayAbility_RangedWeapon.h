// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0Game/Equipment/U0GameplayAbility_FromEquipment.h"
#include "U0GameplayAbility_RangedWeapon.generated.h"

class UU0RangedWeaponInstance;
struct FCollisionQueryParams;

UENUM(BlueprintType)
enum class EU0AbilityTargetingSource : uint8
{
	/* Camera 기준으로 진행 */
	CameraTowardsFocus,
};


/**
 * 
 */
UCLASS()
class U0GAME_API UU0GameplayAbility_RangedWeapon : public UU0GameplayAbility_FromEquipment
{
	GENERATED_BODY()
public:
	struct FRangedWeaponFiringInput
	{
		FVector StartTrace;
		FVector EndAim;
		FVector AimDir;
		UU0RangedWeaponInstance* WeaponData = nullptr;
		bool bCanPlayBulletFX = false;

		FRangedWeaponFiringInput()
			: StartTrace(ForceInitToZero)
			, EndAim(ForceInitToZero)
			, AimDir(ForceInitToZero)
		{ }
	};


public:
	UU0GameplayAbility_RangedWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void StartRangedWeaponTargeting();

	void PerformLocalTargeting(TArray<FHitResult>& OutHits);
	FTransform GetTargetingTransform(APawn* SourcePawn, EU0AbilityTargetingSource Source);
	FVector GetWeaponTargetingSourceLocation() const;
	void TraceBulletsInCartridge(const FRangedWeaponFiringInput& InputData, TArray<FHitResult>& OutHits);
	FHitResult DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHits) const;
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHits) const;
	void AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const;
	ECollisionChannel DetermineTraceChannel(FCollisionQueryParams& TraceParams, bool bIsSimulated) const;
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);
	/* called when target data is ready */
	UFUNCTION(BlueprintImplementableEvent)
	void OnRangeWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	UU0RangedWeaponInstance* GetWeaponInstance();
};
