// Fill out your copyright notice in the Description page of Project Settings.


#include "U0WeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0WeaponInstance)

UU0WeaponInstance::UU0WeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TSubclassOf<UAnimInstance> UU0WeaponInstance::PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const
{
	const FU0AnimLayerSelectionSet& SetToQuery = (bEquipped ? EquippedAnimSet : UnequippedAnimSet);
	return SetToQuery.SelectBestLayer(CosmeticTags);
}
