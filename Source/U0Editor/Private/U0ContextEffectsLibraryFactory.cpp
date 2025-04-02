// Copyright Epic Games, Inc. All Rights Reserved.

#include "U0ContextEffectsLibraryFactory.h"

#include "U0Game/Feedback/ContextEffects/U0ContextEffectsLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0ContextEffectsLibraryFactory)

class FFeedbackContext;
class UClass;
class UObject;

UU0ContextEffectsLibraryFactory::UU0ContextEffectsLibraryFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UU0ContextEffectsLibrary::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UU0ContextEffectsLibraryFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UU0ContextEffectsLibrary* U0ContextEffectsLibrary = NewObject<UU0ContextEffectsLibrary>(InParent, Name, Flags);

	return U0ContextEffectsLibrary;
}
