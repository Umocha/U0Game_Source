// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularAIController.h"
#include "U0AIController.generated.h"

class UU0AbilitySystemComponent;
class AU0AIPlayerState;

/**
 * 
 */
UCLASS(Blueprintable)
class U0GAME_API AU0AIController : public AModularAIController
{
	GENERATED_BODY()
public:
	AU0AIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	AU0AIPlayerState* GetU0AIPlayerState() const;

	UU0AbilitySystemComponent* GetU0AbilitySystemComponent() const;


public:
	/*
	* AController interface
	*/
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;

	virtual void SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay);
	virtual void ClearFocus(EAIFocusPriority::Type InPriority);

};
