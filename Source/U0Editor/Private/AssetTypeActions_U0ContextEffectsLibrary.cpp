// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions_U0ContextEffectsLibrary.h"

#include "U0Game/Feedback/ContextEffects/U0ContextEffectsLibrary.h"

class UClass;

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_U0ContextEffectsLibrary::GetSupportedClass() const
{
	return UU0ContextEffectsLibrary::StaticClass();
}

#undef LOCTEXT_NAMESPACE
