// Fill out your copyright notice in the Description page of Project Settings.


#include "U0CharacterRosterSubsystem.h"
#include "U0Game/Character/U0CharacterData.h"
#include "U0Game/Player/U0PlayerState.h"
#include "U0PartyManagementComponent.h"

UU0CharacterRosterSubsystem::UU0CharacterRosterSubsystem()
{
}

void UU0CharacterRosterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	OwnedCharacters.Empty();
}

void UU0CharacterRosterSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UU0CharacterRosterSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void UU0CharacterRosterSubsystem::AddCharacter(UU0CharacterData* NewCharacter)
{
	OwnedCharacters.Add(NewCharacter);
}

void UU0CharacterRosterSubsystem::SetPartyList(TArray<FName> CharacterNames)
{
	TArray<const UU0CharacterData*> PartyList;

	for (FName& CharacterName : CharacterNames)
	{
		if (const UU0CharacterData* OwnedPawnData = FindCharacterByName(CharacterName))
		{
			PartyList.Add(OwnedPawnData);
		}
	}

	CurrentPartyEntries = CharacterNames;

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AU0PlayerState* U0PS = PC->GetPlayerState<AU0PlayerState>())
		{
			if (UU0PartyManagementComponent* PartyMgrComp = U0PS->FindComponentByClass<UU0PartyManagementComponent>())
			{
				PartyMgrComp->SetPartyList(PartyList);
			}
		}
	}
}

TArray<UU0CharacterData*> UU0CharacterRosterSubsystem::GetOwnedCharacters() const
{
	TArray<UU0CharacterData*> Result;

	for (const TObjectPtr<UU0CharacterData>& Data : OwnedCharacters)
	{
		Result.Add(Data.Get());
	}

	return Result;
}

const UU0CharacterData* UU0CharacterRosterSubsystem::FindCharacterByName(FName CharacterName) const 
{
	for (const UU0CharacterData* OwnedPawnData : OwnedCharacters)
	{
		if (CharacterName == OwnedPawnData->PawnName)
		{
			return OwnedPawnData;
		}
	}
	return nullptr;
}

