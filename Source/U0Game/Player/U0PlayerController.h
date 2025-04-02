// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "U0PlayerController.generated.h"

class AU0PlayerState;
class UU0AbilitySystemComponent;
class UU0PartyManagementComponent;
class UU0InputConfig;
struct FInputActionValue;
struct FGameplayTag;

/**
 * 
 */
UCLASS()
class U0GAME_API AU0PlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
public:
	AU0PlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* PlayerController interface
	*/
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void SetupInputComponent() override;
	virtual void SetViewTarget(class AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams()) override;
	virtual void OnPossess(APawn* PawnToPossess) override;


	void InitializePlayerInput(const UU0InputConfig* InputConfig);
	void Input_Move(const FInputActionValue& InputActionValue);
	void Release_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_SelectPartySlot(const FInputActionValue& InputActionValue);
	void Release_SelectPartySlot(const FInputActionValue& InputActionValue);
	void Input_ZoomInOutMouse(const FInputActionValue& InputActionValue);
	void ShowMouseCursor();
	void HideMouseCursor();

	UFUNCTION(BlueprintCallable)
	void SelectPartySlot(const int32 SelectNum);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void SetU0PartyManagementComponent(UU0PartyManagementComponent* InU0PartyManagementComponent);


	UFUNCTION(BlueprintCallable)
	FVector2D GetInputDir() const { return InputDir; }

	/*
	* member methods
	*/
	AU0PlayerState* GetU0PlayerState() const;
	UU0AbilitySystemComponent* GetU0AbilitySystemComponent() const;


	UPROPERTY()
	TWeakObjectPtr<UU0PartyManagementComponent> U0PartyManagementComponent;


	FVector2D InputDir = FVector2D::Zero();
};
