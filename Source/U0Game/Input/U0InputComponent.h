// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "U0InputConfig.h"
#include "EnhancedInputComponent.h"
#include "U0InputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;

/**
 * 
 */
UCLASS()
class U0GAME_API UU0InputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	UU0InputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* member methods
	*/
	void AddInputMappings(const UU0InputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UU0InputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	template <class UserClass, typename FuncType>
	void BindNativeAction(const UU0InputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UU0InputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);
};

template<class UserClass, typename FuncType>
void UU0InputComponent::BindNativeAction(const UU0InputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);

	// 여기서 알 수 있듯이, InputConfig는 활성화 가능한 InputAction을 담고 있다.
	// - 만약 InputConfig에 없는 InputAction을 Binding시키면, nullptr을 반환하여 바인딩하는데 실패한다!
	TArray<const UInputAction*> InputActions;
	InputConfig->FindNativeInputActionsForTag(InputTag, InputActions, bLogIfNotFound);
	for (const UInputAction* IA : InputActions)
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UU0InputComponent::BindAbilityActions(const UU0InputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	// AbilityAction에 대해서는 그냥 모든 InputAction에 다 바인딩 시킨다!
	for (const FU0InputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
