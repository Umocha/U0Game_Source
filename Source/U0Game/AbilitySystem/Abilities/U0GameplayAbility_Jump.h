// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "U0GameplayAbility.h"
#include "U0GameplayAbility_Jump.generated.h"

/* forward declaration */
class UObject;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

/**
 * Gameplay ability used for character jumping.
 */
UCLASS(Abstract)
class U0GAME_API UU0GameplayAbility_Jump : public UU0GameplayAbility
{
	GENERATED_BODY()
public:
	UU0GameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "U0|Ability")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category = "U0|Ability")
	void CharacterJumpStop();
};
