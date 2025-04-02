// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"

#include "U0VerbMessage.generated.h"

// Represents a generic message of the form Instigator Verb Target (in Context, with Magnitude)
USTRUCT(BlueprintType)
struct FU0VerbMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	FGameplayTag Verb;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	TObjectPtr<UObject> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	TObjectPtr<UObject> Target = nullptr;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	FGameplayTagContainer InstigatorTags;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	FGameplayTagContainer TargetTags;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	FGameplayTagContainer ContextTags;

	UPROPERTY(BlueprintReadWrite, Category=Gameplay)
	double Magnitude = 1.0;

	// Returns a debug string representation of this message
	U0GAME_API FString ToString() const;
};

USTRUCT(BlueprintType)
struct FU0RequestRespawnMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	TObjectPtr<UAbilitySystemComponent> RequesterAbilitySystemComponent = nullptr;
};