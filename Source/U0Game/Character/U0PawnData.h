// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "U0PawnData.generated.h"

class UU0CameraMode;
class UU0InputConfig;
class UU0AbilityTagRelationshipMapping;
class UU0AbilitySet;


USTRUCT(BlueprintType)
struct FPawnAttributeData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthSet")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthSet")
	float MaxHealth;
};

/**
 * 
 */
UCLASS(BlueprintType)
class U0GAME_API UU0PawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UU0PawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* Pawn의 Name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|Pawn")
	FName PawnName;

	/* Pawn의 Class*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|Pawn")
	TSubclassOf<APawn> PawnClass;

	/* Camera Mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|Camera")
	TSubclassOf<UU0CameraMode> DefaultCameraMode;

	/* Input configuration used by player controlled pawns to create input mappings and bind input actions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|InputConfig")
	TObjectPtr<UU0InputConfig> InputConfig;

	/* 해당 Pawn의 Ability System에 허용할 AbilitySet */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|Abilities")
	TArray<TObjectPtr<UU0AbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|Abilities")
	TObjectPtr<UU0AbilityTagRelationshipMapping> TagRelationshipMapping;

	/* 해당 Pawn의 AttributeSet에 설정할 Data */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|Attributes")
	FPawnAttributeData PawnAttributeData;
};
