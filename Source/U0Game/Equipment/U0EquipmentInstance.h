// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "U0EquipmentInstance.generated.h"

struct FU0EquipmentActorToSpawn;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class U0GAME_API UU0EquipmentInstance : public UObject
{
	GENERATED_BODY()
public:
	UU0EquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/*
	* Blueprint 정의를 위한 Equip/Unequip 함수
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnUnequipped"))
	void K2_OnUnequipped();

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	UFUNCTION(BlueprintPure, Category = "Equipment")
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	UObject* GetInstigator() const { return Instigator; }

	void SpawnEquipmentActors(const TArray<FU0EquipmentActorToSpawn>& ActorsToSpawn);
	void DestroyEquipmentActors();

	/*
	* DeterminesOutputType은 C++ 정의에는 APawn* 반환하지만, BP에서는 PawnType에 따라 OutputType이 결정되도록 리다이렉트(Redirect
	*/
	UFUNCTION(BlueprintPure, Category = "Equipment", meta = (DeterminesOutputType = "PawnType"))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;
	/*
	* interfaces
	*/
	virtual void OnEquipped();
	virtual void OnUnequipped();

	/* 어떤 InventoryItemInstance에 의해 활성화 되었는지 (추후, QuickBarComponent에서 보게 될 것이다) */
	UPROPERTY()
	TObjectPtr<UObject> Instigator;

	/* U0EquipmentDefinition에 맞게 Spawn된 Actor Instance들 */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
