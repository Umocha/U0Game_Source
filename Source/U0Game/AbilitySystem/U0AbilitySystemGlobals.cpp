// Copyright Epic Games, Inc. All Rights Reserved.

#include "U0AbilitySystemGlobals.h"

#include "U0GameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0AbilitySystemGlobals)

struct FGameplayEffectContext;

UU0AbilitySystemGlobals::UU0AbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UU0AbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FU0GameplayEffectContext();
}

