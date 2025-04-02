// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "U0HeroStatSetBase.generated.h"

/**
 * 
 */
UCLASS()
class U0GAME_API UU0HeroStatSetBase : public UU0AttributeSet
{
	GENERATED_BODY()
public:
	UU0HeroStatSetBase();


	virtual float GetConcertoEnergyEfficiency() const { return 0; }
	virtual float GetUltimateEnergyEfficiency() const { return 0; }

	virtual void SetConcertoEnergyEfficiency(float NewVal) {}
	virtual void SetUltimateEnergyEfficiency(float NewVal) {}

	virtual FGameplayAttribute GetConcertoEnergyEfficiencyAttribute() const { return FGameplayAttribute(); }
	virtual FGameplayAttribute GetUltimateEnergyEfficiencyAttribute() const { return FGameplayAttribute(); }


};

////////////////////////////////////////
// U0HeroStatSet Slot
////////////////////////////////////////

UCLASS()
class U0GAME_API UU0HeroStatSet1 : public UU0HeroStatSetBase
{
	GENERATED_BODY()
public:
	ATTRIBUE_ACCESSORS(UU0HeroStatSet1, ConcertoEnergyEfficiency_);
	ATTRIBUE_ACCESSORS(UU0HeroStatSet1, UltimateEnergyEfficiency_);

	virtual float GetConcertoEnergyEfficiency() const { return GetConcertoEnergyEfficiency_(); }
	virtual float GetUltimateEnergyEfficiency() const { return GetUltimateEnergyEfficiency_(); }

	virtual void SetConcertoEnergyEfficiency(float NewVal) { SetConcertoEnergyEfficiency_(NewVal); }
	virtual void SetUltimateEnergyEfficiency(float NewVal) { SetUltimateEnergyEfficiency_(NewVal); }

	virtual FGameplayAttribute GetConcertoEnergyEfficiencyAttribute() const { return GetConcertoEnergyEfficiency_Attribute(); }
	virtual FGameplayAttribute GetUltimateEnergyEfficiencyAttribute() const { return GetUltimateEnergyEfficiency_Attribute(); }


	UPROPERTY(BlueprintReadOnly, Category = "U0|Stat")
	FGameplayAttributeData ConcertoEnergyEfficiency_;

	UPROPERTY(BlueprintReadOnly, Category = "U0|Stat")
	FGameplayAttributeData UltimateEnergyEfficiency_;
};
