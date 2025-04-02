// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerStateComponent.h"
#include "U0PartyManagementComponent.generated.h"

/* forward declaration */
class AU0Character;
class UU0CharacterData;
class AU0PlayerState;
class AU0PlayerController;
class UU0AbilitySystemComponent;
struct FInputActionValue;


USTRUCT(BlueprintType)
struct FPartyEntry
{
	GENERATED_BODY()
public:
	FPartyEntry()
		: CharacterData(nullptr)
		, SpawnedPawn(nullptr)
	{}

	FPartyEntry(UU0CharacterData* InCharacterData, ACharacter* InSpawnedPawn)
		: CharacterData(InCharacterData)
		, SpawnedPawn(InSpawnedPawn)
	{
	}

	bool operator==(const FPartyEntry& Other) { return CharacterData == Other.CharacterData; }
	bool operator!=(const FPartyEntry& Other) { return !(operator==(Other)); }

	void SetActivate(bool bIsActivate);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<const UU0CharacterData> CharacterData;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ACharacter> SpawnedPawn;

	UPROPERTY(BlueprintReadOnly)
	bool bActivate = false;
};

/**
 * 
 */
UCLASS()
class U0GAME_API UU0PartyManagementComponent : public UPlayerStateComponent
{
	GENERATED_BODY()
public:
	UU0PartyManagementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* PlayerStateComponent interface */
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializePartyList();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Init Party List"))
	void ReceiveInitializePartyList();

	void SetPartyList(TArray<const UU0CharacterData*> NewPartyList);

	FPartyEntry SpawnEntry(const UU0CharacterData* CharacterData);
	void DestroyEntry(FPartyEntry& Entry);
	void SetSlotEntry(FPartyEntry& Entry, int32 Index);

	UFUNCTION(BlueprintCallable)
	void SetActivePartyIndex(int32 NewIndex, bool IgnoreOutro = false);

	bool CanSwitch(int32 Index);

	UFUNCTION(BlueprintCallable)
	bool IsValidSlotIndex(int32 Index) { return PartyEntries[Index].SpawnedPawn != nullptr; } 

	UFUNCTION(BlueprintCallable)
	void RespawnAtPosition(FVector SpawnPosition);
	bool ReviveSlot(int32 Index);

	void ActivateIndex(int32 Index);
	void DeactivateIndex(int32 Index);
	void OutroActiveCharacter();
	void IntroActiveCharacter();


	/* Input 라우팅 */
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);

protected:
	UFUNCTION()
	void OnRep_Entries();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

	UFUNCTION()
	void OnDeathStarted(AActor* OwningActor);

	UFUNCTION()
	void OnDeathFinished(AActor* OwningActor);

	/* HUD Party Slot 갯수 */
	UPROPERTY()
	int32 NumSlots = 3; 

	/* HUD Party Slot List */
	UPROPERTY()
	TArray<FPartyEntry> PartyEntries;

	/* 현재 활성화된 Slot Index */
	UPROPERTY()
	int32 ActiveSlotIndex = 0;

	UPROPERTY()
	TWeakObjectPtr<AU0PlayerController> U0PlayerController;

	UPROPERTY()
	TWeakObjectPtr<AU0PlayerState> U0PlayerState;

	UPROPERTY()
	TWeakObjectPtr<UU0AbilitySystemComponent> U0AbilitySystemComponent;
};

///////////////////////////////////
// MessageStruct
///////////////////////////////////
USTRUCT(BlueprintType)
struct FU0PartyBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Party)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Party)
	TArray<FPartyEntry> PartyEntries;
};


USTRUCT(BlueprintType)
struct FU0PartyBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Party)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Party)
	int32 ActiveIndex = 0;
};
