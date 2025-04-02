// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0CharacterPartTypes.h"
#include "Components/ControllerComponent.h"
#include "U0ControllerComponent_CharacterParts.generated.h"

class UU0PawnComponent_CharacterParts;


/* ControllerComponent가 소유하는 Character Parts */
USTRUCT()
struct FU0ControllerCharacterPartEntry 
{
	GENERATED_BODY()
	
	/* Character Part에 대한 정의 (MetaData) */
	UPROPERTY(EditAnywhere)
	FU0CharacterPart Part;

	/* Character Part 핸들 (고유값) - Controller가 Possess하고 있는 Pawn에서 생성한(인스턴스) Character Part 핸들값 */
	FU0CharacterPartHandle Handle;
};

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class U0GAME_API UU0ControllerComponent_CharacterParts : public UControllerComponent
{
	GENERATED_BODY()
public:
	UU0ControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UU0PawnComponent_CharacterParts* GetPawnCustomizer() const;

	UFUNCTION(BlueprintCallable, Category = "Cosmetics")
	void AddCharacterPart(const FU0CharacterPart& NewPart);

	void AddCharacterPartInternal(const FU0CharacterPart& NewPart);

	void RemoveAllCharacterParts();

	/* UFUNCTION으로 정의하지 않으면 AddDynamic이 되지 않는다 */
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UPROPERTY(EditAnywhere, Category = "Cosmetics")
	TArray<FU0ControllerCharacterPartEntry> CharacterParts;
};
