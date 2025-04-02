// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "U0Game/AbilitySystem/U0AbilitySet.h"
#include "U0EquipmentManagerComponent.generated.h"

class UU0EquipmentInstance;
class UU0EquipmentDefinition;

USTRUCT(BlueprintType)
struct FU0AppliedEquipmentEntry
{
	GENERATED_BODY()

	/* 장착물에 대한 Meta Data*/
	UPROPERTY()
	TSubclassOf<UU0EquipmentDefinition> EquipmentDefinition;

	/* EquipmentDefinition을 통해 생선된 인스턴스 */
	UPROPERTY()
	TObjectPtr<UU0EquipmentInstance> Instance = nullptr;

	/* 무기에 할당된 허용가능한 GameplayAbility */
	UPROPERTY()
	FU0AbilitySet_GrantedHandles GrantedHandles;

};

/*
* 참고로 EquipmentInstance의 인스턴스를 Entry에서 관리하고 있다:
* - U0EquipmentList는 생성된 객체를 관리한다고 보면 된다
*/
USTRUCT(BlueprintType)
struct FU0EquipmentList
{
	GENERATED_BODY()

	FU0EquipmentList(UActorComponent* InOwnerComponent = nullptr)
		: OwnerComponent(InOwnerComponent)
	{}
	
	UU0EquipmentInstance* AddEntry(TSubclassOf<UU0EquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UU0EquipmentInstance* Instance);

	UU0AbilitySystemComponent* GetAbilitySystemComponent();

	/* 장착물에 대한 관리 리스트 */
	UPROPERTY()
	TArray<FU0AppliedEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

/**
 * Pawn의 Component로서 장착물에 대한 관리를 담당한다
 */
UCLASS()
class U0GAME_API UU0EquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	UU0EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UU0EquipmentInstance* EquipItem(TSubclassOf<UU0EquipmentDefinition> EquipmentDefinition);
	void UnequipItem(UU0EquipmentInstance* ItemInstance);

	/* 장착물 중 처음 것을 반환 없으면 NULL */
	UU0EquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UU0EquipmentInstance> InstanceType);

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable)
	TArray<UU0EquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UU0EquipmentInstance> InstanceType) const;

	UPROPERTY()
	FU0EquipmentList EquipmentList;
};
