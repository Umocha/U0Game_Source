// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "U0CharacterRosterSubsystem.generated.h"

class UU0CharacterData;

/**
 * 
 */
UCLASS(Blueprintable)
class U0GAME_API UU0CharacterRosterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UU0CharacterRosterSubsystem();

	/*
	* UGameInstanceSubsystem's interfaces
	*/
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;


	/*
	* UU0CharacterRosterSubsystem
	*/
	UFUNCTION(BlueprintCallable)
	void AddCharacter(UU0CharacterData* NewCharacter);
	UFUNCTION(BlueprintCallable)
	void SetPartyList(TArray<FName> CharacterNames);

	UFUNCTION(BlueprintCallable)
	TArray<UU0CharacterData*> GetOwnedCharacters() const;

	UFUNCTION(BlueprintCallable)
	const UU0CharacterData* FindCharacterByName(FName CharacterName) const;

	UFUNCTION(BlueprintCallable)
	const TArray<FName>& GetCurrentPartyEntries() const { return CurrentPartyEntries; }

	// 캐릭터 목록
	TArray<TObjectPtr<UU0CharacterData>> OwnedCharacters;

	// 현재 선택된 PartyEntries
	TArray<FName> CurrentPartyEntries;
};
