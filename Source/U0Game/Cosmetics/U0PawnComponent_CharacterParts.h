// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0CharacterPartTypes.h"
#include "U0CosmeticAnimationTypes.h"
#include "Components/PawnComponent.h"
#include "U0PawnComponent_CharacterParts.generated.h"

class UU0PawnComponent_CharacterParts;


/* 인스턴스화 된 Character Part의 단위*/
USTRUCT()
struct FU0AppliedCharacterPartEntry
{
	GENERATED_BODY()

	/* Character Part의 정의(Meta Data) */
	UPROPERTY()
	FU0CharacterPart Part;

	/* U0CharacterPartList에서 할당 받은 Part 핸들값 (FU0AppliedCharacterPartEntry의 Handle값과 같아야 함 -> 같은면 같은 Part) */
	UPROPERTY()
	int32 PartHandle = INDEX_NONE;

	/* 인스턴스화 된 Character Part용 Actor */
	UPROPERTY()
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};


/* U0PawnComponent_CharacterParts에서 실질적 Character Parts를 관리하는 클래스 */
USTRUCT(BlueprintType)
struct FU0CharacterPartList
{
	GENERATED_BODY()
	

	FU0CharacterPartList()
		: OwnerComponent(nullptr)
	{}

	FU0CharacterPartList(UU0PawnComponent_CharacterParts* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

	bool SpawnActorForEntry(FU0AppliedCharacterPartEntry& Entry);
	void DestroyActorForEntry(FU0AppliedCharacterPartEntry& Entry);

	FU0CharacterPartHandle AddEntry(const FU0CharacterPart& NewPart);
	void RemoveEntry(FU0CharacterPartHandle Handle);


	FGameplayTagContainer CollectCombinedTags() const;

	/* 현재 인스턴스화 된 Character Part */
	UPROPERTY()
	TArray<FU0AppliedCharacterPartEntry> Entries;

	/* 해당 U0CharacterPartList의 Owner인 PawnComponent */
	UPROPERTY()
	TObjectPtr<UU0PawnComponent_CharacterParts> OwnerComponent;

	/* 앞서 보았던 PartHandle의 값을 할당 및 관리하는 변수 */
	int32 PartHandleCounter = 0;
};

/**
 * 
 */
UCLASS()
class U0GAME_API UU0PawnComponent_CharacterParts : public UPawnComponent
{
	GENERATED_BODY()
public:
	UU0PawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	USkeletalMeshComponent* GetParentMeshComponent() const;
	USceneComponent* GetSceneComponentToAttachTo() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Cosmetics")
	FGameplayTagContainer GetCombinedTags(FGameplayTag RequiredPrefix) const;

	void BroadcastChanged();

	FU0CharacterPartHandle AddCharacterPart(const FU0CharacterPart& NewPart);
	void RemoveCharacterPart(FU0CharacterPartHandle Handle);

	/* 인스턴스화 된 Character Part */
	UPROPERTY()
	FU0CharacterPartList CharacterPartList;

	/* 애니메이션 적용을 위한 메시와 연결고리 */
	UPROPERTY(EditAnywhere, Category = "Cosmetics")
	FU0AnimBodyStyleSelectionSet BodyMeshes;
};
