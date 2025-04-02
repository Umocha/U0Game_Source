// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0Game/Equipment/U0EquipmentInstance.h"
#include "U0Game/Cosmetics/U0CosmeticAnimationTypes.h"
#include "U0WeaponInstance.generated.h"


/**
 * 
 */
UCLASS()
class U0GAME_API UU0WeaponInstance : public UU0EquipmentInstance
{
	GENERATED_BODY()
public:
	UU0WeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Weapon에 적용할 AnimLayer를 선택하여 반환 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Animation")
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;

	/* Weapon의 Equip/Unequip에 대한 Animation Set 정보를 들고 있다 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FU0AnimLayerSelectionSet EquippedAnimSet; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FU0AnimLayerSelectionSet UnequippedAnimSet;
};
