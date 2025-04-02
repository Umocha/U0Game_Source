// Fill out your copyright notice in the Description page of Project Settings.


#include "U0InventoryManagerComponent.h"
#include "U0InventoryItemInstance.h"
#include "U0InventoryItemDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0InventoryManagerComponent)

//////////////////////////////////////////
// FU0InventoryList
//////////////////////////////////////////
UU0InventoryItemInstance* FU0InventoryList::AddEntry(TSubclassOf<UU0InventoryItemDefinition> ItemDef)
{
	UU0InventoryItemInstance* Result = nullptr;
	check(ItemDef);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FU0InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UU0InventoryItemInstance>(OwningActor);
	NewEntry.Instance->ItemDef = ItemDef;

	Result = NewEntry.Instance;
	return Result;
}

//////////////////////////////////////////
// UU0InventoryManagerComponent
//////////////////////////////////////////
UU0InventoryManagerComponent::UU0InventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
}

UU0InventoryItemInstance* UU0InventoryManagerComponent::AddItemDefinition(TSubclassOf<UU0InventoryItemDefinition> ItemDef)
{
	UU0InventoryItemInstance* Result = nullptr;
	if (ItemDef)
	{
		Result = InventoryList.AddEntry(ItemDef);
	}
	return Result;
}
