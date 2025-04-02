// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"

#include "U0ContextEffectsLibraryFactory.generated.h"

class FFeedbackContext;
class UClass;
class UObject;

UCLASS(hidecategories = Object, MinimalAPI)
class UU0ContextEffectsLibraryFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	//~ Begin UFactory Interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	virtual bool ShouldShowInNewMenu() const override
	{
		return true;
	}
	//~ End UFactory Interface	
};

