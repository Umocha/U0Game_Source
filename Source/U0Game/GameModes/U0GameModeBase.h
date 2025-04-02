// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "U0GameModeBase.generated.h"

class UU0ExperienceDefinition;
class UU0PawnData;

/**
 * 
 */
UCLASS()
class U0GAME_API AU0GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AU0GameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() final;

	/* GetDefaultPawnClassForController */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) final;

	/* HandleStartingNewPlayer */
	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) final;

	/* SpawnDefaultPawnAtTransform */
	APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) final;

	/*
	* member methods
	*/
	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const UU0ExperienceDefinition* CurrentExperience);
	const UU0PawnData* GetPawnDataForController(const AController* InController) const;
};
