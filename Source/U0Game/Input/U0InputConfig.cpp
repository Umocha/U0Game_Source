// Fill out your copyright notice in the Description page of Project Settings.


#include "U0InputConfig.h"
#include "U0Game/U0LogChannels.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(U0InputConfig)

UU0InputConfig::UU0InputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UU0InputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// NativeInputActions을 순회하며, Input으로 들어온 InputTag가 있는지 체크한다:
	// - 있으면, 그에 따른 InputAction을 반환하지만, 없다면 그냥 nullptr을 반환한다
	for (const FU0InputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogU0, Error, TEXT("can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

void UU0InputConfig::FindNativeInputActionsForTag(const FGameplayTag& InputTag,TArray<const UInputAction*>& OutActions, bool bLogNotFound) const
{
	OutActions.Empty();

	for (const FU0InputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			OutActions.Add(Action.InputAction);
		}
	}


	if (!OutActions.Num() && bLogNotFound)
	{
		UE_LOG(LogU0, Error, TEXT("can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return;
}

const UInputAction* UU0InputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FU0InputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogU0, Error, TEXT("can't find AbilityInputActions for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
