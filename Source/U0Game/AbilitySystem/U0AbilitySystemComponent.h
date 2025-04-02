// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "U0AbilitySystemComponent.generated.h"

class UU0HealthSetBase;
class UU0HeroSkillSetBase;
class UU0AbilityTagRelationshipMapping;

/**
 * 
 */
UCLASS()
class U0GAME_API UU0AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UU0AbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* AbilitySystemComponent's interface
	*/
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	void TryActivateAbilitiesOnSpawn();

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;

	virtual int32 HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload);

	/*
	* member methods
	*/
	void AbilityInputTagPressed(const FGameplayTag InputTag);
	void AbilityInputTagReleased(const FGameplayTag InputTag);
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	/** Sets the current tag relationship mapping, if null it will clear it out */
	void AddTagRelationshipMapping(UU0AbilityTagRelationshipMapping* NewMapping);
	void RemoveTagRelationshipMapping(UU0AbilityTagRelationshipMapping* Mapping);

	/** Looks at ability tags and gathers additional required and blocking tags */
	void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const;

	void ActivateAbilityInSlot(int32 NewIndex);

	const UU0HealthSetBase* GetHealthSet(int32 SlotIndex = INDEX_NONE) const;
	const UU0HeroSkillSetBase* GetSkillSet(int32 SlotIndex = INDEX_NONE) const;

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CancelAbilitiesWithTags(const FGameplayTagContainer& TagsToCancel);

	UFUNCTION(BlueprintCallable)
	int32 GetActiveIndex() const { return ActiveIndex; }

protected:
	// If set, this table is used to look up tag relationships for activate and cancel
	UPROPERTY()
	TArray<TObjectPtr<UU0AbilityTagRelationshipMapping>> TagRelationshipMappings;

	/* Ability Input 처리할 Pending Queue*/
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	int32 ActiveIndex = -1;
};
