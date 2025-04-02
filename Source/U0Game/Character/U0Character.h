// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "AbilitySystemInterface.h"
#include "U0Character.generated.h"

/* forward declarations */
class UU0PawnExtensionComponent;
class UU0CameraComponent;
class UU0HealthComponent;
class UU0SkillComponent;
class UU0AbilitySystemComponent;

UENUM(Blueprintable)
enum class EHeroAttribute : uint8
{
	None = 0,
	Red,
	Blue,
	Green
};

UCLASS()
class U0GAME_API AU0Character : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AU0Character();


	void OnAbilitySystemInitialized();
	void OnAbilitySystemUninitialized();

	/*
	* IAbilitySystemInterface
	*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFUNCTION(BlueprintCallable, Category = "U0|Character")
	UU0AbilitySystemComponent* GetU0AbilitySystemComponent() const;

	/*
	* ACharacter Interface
	*/
	virtual void Landed(const FHitResult& Hit) override;

	/*
	* member methord
	*/
	UFUNCTION(BlueprintCallable)
	int32 GetSlotIndex() const { return SlotIndex; }
	void SetSlotIndex(int32 Index) { SlotIndex = Index; }

	UFUNCTION(BlueprintCallable)
	EHeroAttribute GetHeroAttribute() const { return  Attribute; }

protected:
	void InitializeGameplayTags();

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "U0|Character")
	TObjectPtr<UU0PawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "U0|Character")
	TObjectPtr<UU0CameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "U0|Character")
	TObjectPtr<UU0HealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "U0|Character")
	TObjectPtr<UU0SkillComponent> SkillComponent;

protected:
	int32 SlotIndex = -1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "U0|Character")
	EHeroAttribute Attribute = EHeroAttribute::None;
};
