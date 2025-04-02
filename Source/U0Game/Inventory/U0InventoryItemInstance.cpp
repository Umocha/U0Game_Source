// Fill out your copyright notice in the Description page of Project Settings.


#include "U0InventoryItemInstance.h"
#include "U0InventoryItemDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0InventoryItemInstance)

UU0InventoryItemInstance::UU0InventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UU0InventoryItemFragment* UU0InventoryItemInstance::FindFragmentByClass(TSubclassOf<UU0InventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && FragmentClass != nullptr)
	{
		// UU0InventoryItemDefinition은 멤버 변수가 EditDefaultsOnlt로 선언되어 있으므로, GetDefault로 가져와도 무관하다
		// - Fragment 정보는 Instance가 아닌 Definition에 있다
		return GetDefault<UU0InventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}
