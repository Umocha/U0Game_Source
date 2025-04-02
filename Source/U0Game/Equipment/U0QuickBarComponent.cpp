// Fill out your copyright notice in the Description page of Project Settings.


#include "U0QuickBarComponent.h"
#include "U0EquipmentManagerComponent.h"
#include "U0EquipmentInstance.h"
#include "U0EquipmentDefinition.h"
#include "U0Game/Inventory/U0InventoryItemInstance.h"
#include "U0Game/Inventory/U0InventoryFragment_EquippableItem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0QuickBarComponent)

UU0QuickBarComponent::UU0QuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UU0QuickBarComponent::BeginPlay()
{
	// NumSlots에 따라 미리 Slots을 할당해준다
	// 이 부분 왜이렇게 했는지 잘모르겠는데
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	Super::BeginPlay();
}

UU0EquipmentManagerComponent* UU0QuickBarComponent::FindEquipmentManager() const
{
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = OwnerController->GetPawn())
		{
			return Pawn->FindComponentByClass<UU0EquipmentManagerComponent>();
		}
	}
	return nullptr;
}

void UU0QuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	// 현재 활성화된 ActiveSlotIndex를 활용하여 활성화된 InventoryItemInstance를 찾는다
	if (UU0InventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		// Slot Item을 통해 (InventoryItemInstance) InventoryFragment_EquippableItem의 Fragment를 찾는다:
		// - 찾는 것을 실패했다면, 장착할 수 없는 Inventory Item임을 의미한다
		if (const UU0InventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UU0InventoryFragment_EquippableItem>())
		{
			// EquippableItem에서 EquipmentDefinition을 찾는다:
			// - EquipmentDefinition이 있어야, 장착 가능
			TSubclassOf<UU0EquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef)
			{
				// 아래는 Unequip이랑 비슷하게 EquipmentManager를 통해 장착한다
				if (UU0EquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					EquippedItem = EquipmentManager->EquipItem(EquipDef);
					if (EquippedItem)
					{
						// EquippedItem에는 앞서 보았던 Instiagor로 Slot을 대상으로 넣는다
						EquippedItem->Instigator = SlotItem;
					}
				}
			}
		}
	}
}

void UU0QuickBarComponent::UnequipItemInSlot()
{
	// 참고로 QuickBar는 Controller에 붙어있는 Component이지만, EquipmentManagerComponent는 Controller가 Possess하고있는 Pawn의 Component이다
	if (UU0EquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		// 현재 장착된 Item이 있다면,
		if (EquippedItem)
		{
			// EquipmentManager를 통해, Pawn의 장비를 해제시킨다
			EquipmentManager->UnequipItem(EquippedItem);

			// 그리고 Controller에도 EquipItem의 상태를 없는 것으로 업데이트한다
			EquippedItem = nullptr;
		}
	}
}

void UU0QuickBarComponent::AddItemToSlot(int32 SlotIndex, UU0InventoryItemInstance* Item)
{
	// 해당 로직을 보면, Slots는 Add로 동적 추가가 아닌, Index에 바로 넣는다:
	// - 그럼 미리 Pre-size 했다는 것인데 이는 BeginPlay()에서 진행한다
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
		}
	}
}

void UU0QuickBarComponent::SetActiveSlotIndex(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		// EquipItem/UnequipItem과 NewIndex를 통해 할당된 Item을 장착 및 업데이트 진행한다
		UnequipItemInSlot();
		ActiveSlotIndex = NewIndex;
		EquipItemInSlot();
	}
}
