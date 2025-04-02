// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0PawnData.h"
#include "Styling/SlateBrush.h"
#include "U0CharacterData.generated.h"

USTRUCT(BlueprintType)
struct FCharacterAttributeData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillSet")
	float ConcertoEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillSet")
	float MaxConcertoEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillSet")
	float UltimateEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillSet")
	float MaxUltimateEnergy;
};


/**FCharacterAttributeData
 * 
 */
UCLASS(BlueprintType)
class U0GAME_API UU0CharacterData : public UU0PawnData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|Attributes")
	FCharacterAttributeData CharacterAttributeData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|Icon")
	FSlateBrush CharacterBrush;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|Icon")
	FSlateBrush SkillBrush;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|Icon")
	FSlateBrush UltimateBrush;
};
