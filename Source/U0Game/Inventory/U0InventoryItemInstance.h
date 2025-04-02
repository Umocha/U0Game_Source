// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "U0InventoryItemInstance.generated.h"

class UU0InventoryItemDefinition;
class UU0InventoryItemFragment;

/**
 * 
 */
UCLASS(BlueprintType)
class U0GAME_API UU0InventoryItemInstance : public UObject
{
	GENERATED_BODY()
public:
	UU0InventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = "FragmentClass"))
	const UU0InventoryItemFragment* FindFragmentByClass(TSubclassOf<UU0InventoryItemFragment> FragmentClass) const;

	template<typename ResultClass>
	const ResultClass* FindFragmentByClass()const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

	/* Inventory Item의 인스턴스에는 무엇으로 정의되었는지 메타 클래스인 U0InventoryItemDefinition을 들고 있다 */
	UPROPERTY()
	TSubclassOf<UU0InventoryItemDefinition> ItemDef;



};
