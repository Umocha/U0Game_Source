// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "U0InventoryItemDefinition.generated.h"

/*
* Inventory에 대한 Frament은 확 와닿지 않을 수 있다:
* - Lyra에서 사용하는 예시를 통해 이해해보자:
*     - ULyraInventoryFragment_EquippableItem은 EquipmentItemDefinition을 가지고 있으면, 장착 가능한 아이템을 의미한다
*     - ULyraInventoryFragment_SetStats는 아이템에 대한 정보를 가지고 있다
*       - Rifle에 대한 SetStats으로 총알(Ammo)에 대한 장착 최대치와 현재 남은 잔탄 수를 예시로 들 수 있다
*/
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class UU0InventoryItemFragment : public UObject
{
	GENERATED_BODY()
public:

};

/**
 * 
 */
UCLASS(Blueprintable)
class U0GAME_API UU0InventoryItemDefinition : public UObject
{
	GENERATED_BODY()
public:
	UU0InventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UU0InventoryItemFragment* FindFragmentByClass(TSubclassOf<UU0InventoryItemFragment> FragmentClass) const;

	/* Inventory Item 정의(메타) 이름 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	FText DisplayName;

	/* Inventory Item의 Component를 Fragment로 인식하면 된다 */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Display")
	TArray<TObjectPtr<UU0InventoryItemFragment>> Fragments;
};
