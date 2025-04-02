// Fill out your copyright notice in the Description page of Project Settings.


#include "U0ControllerComponent_CharacterParts.h"
#include "U0PawnComponent_CharacterParts.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0ControllerComponent_CharacterParts)


UU0ControllerComponent_CharacterParts::UU0ControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UU0ControllerComponent_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (AController* OwningController = GetController<AController>())
		{
			OwningController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
		}
	}
}

void UU0ControllerComponent_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllCharacterParts();
	Super::EndPlay(EndPlayReason);
}

UU0PawnComponent_CharacterParts* UU0ControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// 생각해보면, 우리는 앞서 U0PawnComponent_CharacterParts를 상속받는 B_MannequinPawnCosmetics를 이미 B_Hero_ShooterMannequin에 추가했다
		// B_MannequinPawnCosmetics를 반환되길 기대한다
		return ControlledPawn->FindComponentByClass<UU0PawnComponent_CharacterParts>();
	}

	return nullptr;
}

void UU0ControllerComponent_CharacterParts::AddCharacterPart(const FU0CharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart);
}

void UU0ControllerComponent_CharacterParts::AddCharacterPartInternal(const FU0CharacterPart& NewPart)
{
	FU0ControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();
	NewEntry.Part = NewPart;

	if(UU0PawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
	}
}

void UU0ControllerComponent_CharacterParts::RemoveAllCharacterParts()
{
	if (UU0PawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		for (FU0ControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}
	CharacterParts.Reset();
}

void UU0ControllerComponent_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// 이전 OldPawn에 대해서는 Character Parts를 전부 제거해주자
	if (UU0PawnComponent_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<UU0PawnComponent_CharacterParts>() : nullptr)
	{
		for (FU0ControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
		}
	}

	// 새로운 Pawn에 대해서 기존 Controller가 가지고 있는 Character Parts를 추가해주자
	if (UU0PawnComponent_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<UU0PawnComponent_CharacterParts>() : nullptr)
	{
		for (FU0ControllerCharacterPartEntry& Entry : CharacterParts)
		{
			check(!Entry.Handle.IsValid());
			Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
		}
	}



}
