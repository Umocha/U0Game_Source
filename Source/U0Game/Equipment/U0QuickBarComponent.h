// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "U0QuickBarComponent.generated.h"

class UU0InventoryItemInstance;
class UU0EquipmentInstance;
class UU0EquipmentManagerComponent;

/**
 * HUD의 QuickBar를 생각하면 된다:
 * - 흔히 MMORPG에서는 ShortCut HUD를 연상하면 된다
 * 
 * 해당 Component는 ControllerComponent로서, PlayerController에 의해 조작계 중 하나로 생각해도 된다
 * - HUD(Slate)와 Inventory/Equipment(Gameplay)의 다리(Bridge) 역할하는 Component로 생각하자
 * - 해당 Component는 Lyra의 HUD 및 Slate Widget을 다루면서 다시 보게 될 거임
 */
UCLASS()
class U0GAME_API UU0QuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()
public:
	UU0QuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* ControllerComponent interface */
	virtual void BeginPlay() override;

	/*
	* member methods
	*/
	UU0EquipmentManagerComponent* FindEquipmentManager() const;
	void EquipItemInSlot();
	void UnequipItemInSlot();

	UFUNCTION(BlueprintCallable)
	void AddItemToSlot(int32 SlotIndex, UU0InventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, Category = "U0")
	void SetActiveSlotIndex(int32 NewIndex);


	/* HUD QuickBar Slot 갯수 */
	UPROPERTY()
	int32 NumSlots = 3;

	/* HUD QuickBar Slot 리스트 */
	UPROPERTY()
	TArray<TObjectPtr<UU0InventoryItemInstance>> Slots;

	/* 현재 활성화된 Slot Index (아마 Lyra는 딱 한개만 Slot이 활성화되는가보다?) */
	UPROPERTY()
	int32 ActiveSlotIndex = -1;

	/* 현재 장착한 장비 정보 */
	// 이건 왜 리스트가 아님? 선택한 무기를 뜻하는건가? 장착한 장비가 아니라 -> 아마 그런듯
	UPROPERTY()
	TObjectPtr<UU0EquipmentInstance> EquippedItem;
};
