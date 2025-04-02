// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularPlayerState.h"
#include "U0PlayerState.generated.h"

class UU0ExperienceDefinition;
class UU0PawnData;
class UU0AbilitySystemComponent;

/**
 * 
 */
UCLASS()
class U0GAME_API AU0PlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AU0PlayerState(const  FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	/*
	* AActor's interface
	*/
	virtual void PostInitializeComponents() final;

	/*
	* member methods
	*/
	template<class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void OnExperienceLoaded(const UU0ExperienceDefinition* CurrentExperience);
	void SetPawnData(const UU0PawnData* InPawnData);
	UU0AbilitySystemComponent* GetU0AbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	static const FName NAME_U0AbilityReady;

	int32 GetNumPartySlots() const { return NumPartySlots; }


protected:
	UPROPERTY()
	TObjectPtr<const UU0PawnData> PawnData;

	UPROPERTY(VisibleAnywhere, Category = "U0|PlayerState")
	TObjectPtr<UU0AbilitySystemComponent> AbilitySystemComponent;

	int32 NumPartySlots = 3;

};
