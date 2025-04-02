// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "U0EquipmentDefinition.generated.h"

class UU0EquipmentInstance;
class UU0AbilitySet;

USTRUCT()
struct FU0EquipmentActorToSpawn
{
	GENERATED_BODY()

	/* Spawn할 대상 Actor 클래스 (== Actor를 상속받은 Asset으로 생각해도 됨) */
	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<AActor> ActorToSpawn;

	/* 어느 Bone Socket에 붙일지 결정한다 */
	UPROPERTY(EditAnywhere, Category = "Equipment")
	FName AttachSocket;

	/* Socket에 Transform Offset */
	UPROPERTY(EditAnywhere, Category = "Equipment")
	FTransform AttachTransform;
};

/**
 * UU0EquipmentDefinition은 장착 아이템에 대한 정의 클래스(Meta Data)이다
 */
UCLASS(Blueprintable)
class U0GAME_API UU0EquipmentDefinition : public UObject
{
	GENERATED_BODY()
public:
	UU0EquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* 해당 Meta Data를 사용하면, 어떤 인스턴스를 Spawn할지 결정하는 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TSubclassOf<UU0EquipmentInstance> InstanceType;

	/* 해당 장착 아이템을 사용하면, 어떤 Actor가 Spawn이 되는지 정보를 담고 있다 */
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<FU0EquipmentActorToSpawn> ActorsToSpawn;

	/* 장착을 통해 부여 가능한 Ability Set*/
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	TArray<TObjectPtr<UU0AbilitySet>> AbilitySetsToGrant;
};
