// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "U0InputConfig.generated.h"

class UInputAction;

/* U0InputAction은 GameplayTag와 InputAction을 연결하는 래퍼 struct */
USTRUCT(BlueprintType)
struct FU0InputAction 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


/**
 * 
 */
UCLASS()
class U0GAME_API UU0InputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UU0InputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;
	void FindNativeInputActionsForTag(const FGameplayTag& InputTag, TArray<const UInputAction*>& OutActions, bool bLogNotFound) const;

	/*
	* member variables
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty ="InputAction"))
	TArray<FU0InputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputAction"))
	TArray<FU0InputAction> AbilityInputActions;
};
