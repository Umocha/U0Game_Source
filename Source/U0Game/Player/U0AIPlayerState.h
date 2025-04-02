// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "U0AIPlayerState.generated.h"

class UU0PawnData;
class UU0AbilitySystemComponent;

/**
 * 
 */
UCLASS()
class U0GAME_API AU0AIPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AU0AIPlayerState(const  FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* AActor's interface
	*/
	virtual void PostInitializeComponents() final;

	/*
	* member methods
	*/
	void SetPawnData(const UU0PawnData* InPawnData);
	UU0AbilitySystemComponent* GetU0AbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


protected:
	UPROPERTY()
	TObjectPtr<const UU0PawnData> PawnData;

	UPROPERTY(VisibleAnywhere, Category = "U0|PlayerState")
	TObjectPtr<UU0AbilitySystemComponent> AbilitySystemComponent;

};
