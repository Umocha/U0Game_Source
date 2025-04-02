// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "U0GameplayAbility.generated.h"

/* forward declaration */
class UU0AbilitySystemComponent;
class AU0PlayerController;
class UU0HeroComponent;
class AU0Monster;

UENUM(BlueprintType)
enum class  EU0AbilityActivationPolicy : uint8
{
	/* Input이 Triggered 되었을 경우 (Pressed/Released) */
	OnInputTriggered,
	/* Input이 Held 되었을 경우 */
	WhileInputActive,
	/* avatar가 생성되었을 경우, 바로 할당 */
	OnSpawn,
};

/**
 * 
 */
UCLASS(Abstract)
class U0GAME_API UU0GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UU0GameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


	UFUNCTION(BlueprintCallable, Category = "U0|Ability")
	UU0AbilitySystemComponent* GetU0AbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Ability")
	AU0PlayerController* GetU0PlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Ability")
	AU0Character* GetU0CharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Ability")
	UU0HeroComponent* GetHeroComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Ability")
	AU0Monster* GetU0MonsterFromActorInfo() const;

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	/*
	* UGameplayAbility interface
	*/
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;


	virtual void OnPawnAvatarSet();


	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
	void K2_OnPawnAvatarSet();

	/*
	* member variables
	*/
	/* 언제 GA가 활성화 될지 정책 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "U0|AbilityActivation")
	EU0AbilityActivationPolicy ActivationPolicy;
};
