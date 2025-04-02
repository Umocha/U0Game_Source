// Fill out your copyright notice in the Description page of Project Settings.


#include "U0InventoryItemDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0InventoryItemDefinition)

UU0InventoryItemDefinition::UU0InventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UU0InventoryItemFragment* UU0InventoryItemDefinition::FindFragmentByClass(TSubclassOf<UU0InventoryItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		// Fragments를 순회하며, IsA()를 통해 해당 클래스를 가지고 있는지 확인한다:
		for (UU0InventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}
