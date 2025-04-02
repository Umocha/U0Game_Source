// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "U0InventoryManagerComponent.generated.h"

class UU0InventoryItemInstance;
class UU0InventoryItemDefinition;


/* Inventory Item 단위 객체 */
USTRUCT(BlueprintType)
struct FU0InventoryEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UU0InventoryItemInstance> Instance = nullptr;
};

/* Inventory Item 관리 객체 */
USTRUCT(BlueprintType)
struct FU0InventoryList
{
	GENERATED_BODY()

	FU0InventoryList(UActorComponent* InOwnerComponent = nullptr)
		: OwnerComponent(InOwnerComponent)
	{}

	UU0InventoryItemInstance* AddEntry(TSubclassOf<UU0InventoryItemDefinition> ItemDef);

	UPROPERTY()
	TArray<FU0InventoryEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

/*
* PlayerController의 Component로서 Inventory를 관리한다
* - 사실 UActorComponent 상속이 아닌 UControllerComponent를 상속받아도 될거 같은데? -> 일단 Lyra 기준으로 UActorComponent를 상속받고 있다
*/

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class U0GAME_API UU0InventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()	
public:	
	UU0InventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* InventoryItemDefinition을 통해, InventoryList에 추가하여 관리하며, InventoryItemInstance를 반환한다 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UU0InventoryItemInstance* AddItemDefinition(TSubclassOf<UU0InventoryItemDefinition> ItemDef);

	UPROPERTY()
	FU0InventoryList InventoryList; 

};
