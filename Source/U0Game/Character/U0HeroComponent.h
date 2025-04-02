// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "U0Game/GameFeatures/GameFeatureAction_AddInputContextMapping.h"
#include "U0HeroComponent.generated.h"

class UU0CameraMode;
class UU0InputConfig;
struct FInputActionValue;
struct FU0MappableConfigPair;

/**
 * component that sets up input and camera handling for player controlled pawns (or bots that simulate players)
 * - this depends on a PawnExtensionComponent to coordinate initialization
 * 
 * 카메라, 입력 등 플레이어가 제어하는 시스템의 초기화를 처리하는 컴포넌트
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class U0GAME_API UU0HeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UU0HeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Lyra|Hero")
	static UU0HeroComponent* FindHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UU0HeroComponent>() : nullptr); }

	/* FeatureName 정의	*/
	static const FName NAME_ActorFeatureName;

	/* Extension Event 이름 정의 */
	static const FName NAME_BindInputsNow;

	/*
	* UPawnComponent interfaces
	*/
	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	/*
	* IGameFrameworkInitStateInterface
	*/
	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; }
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) final;
	virtual void CheckDefaultInitialization() final;

	/*
	* member methods
	*/
	TSubclassOf<UU0CameraMode> DetermineCameraMode() const;
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	/** Adds mode-specific input config */
	void AddAdditionalInputConfig(const UU0InputConfig* InputConfig);

	/** Removes a mode-specific input config if it has been added */
	void RemoveAdditionalInputConfig(const UU0InputConfig* InputConfig);

	/** True if this is controlled by a real player and has progressed far enough in initialization where additional input bindings can be added */
	bool IsReadyToBindInputs() const;

	/*
	* member variables
	*/
	UE_DEPRECATED(5.3, "DefaultInputConfigs have been deprecated, use DefaultInputMappings instead")
	TArray<FU0MappableConfigPair> DefaultInputConfigs;

	UPROPERTY(EditAnywhere)
	TArray<FInputMappingContextAndPriority> DefaultInputMappings;

	/** True when player input bindings have been applied, will never be true for non - players */
	bool bReadyToBindInputs;
};
