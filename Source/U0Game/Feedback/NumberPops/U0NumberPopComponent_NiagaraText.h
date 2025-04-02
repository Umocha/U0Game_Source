// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "U0NumberPopComponent.h"

#include "U0NumberPopComponent_NiagaraText.generated.h"

class UU0DamagePopStyleNiagara;
class UNiagaraComponent;
class UObject;

UCLASS(Blueprintable)
class UU0NumberPopComponent_NiagaraText : public UU0NumberPopComponent
{
	GENERATED_BODY()

public:

	UU0NumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UU0NumberPopComponent interface
	virtual void AddNumberPop(const FU0NumberPopRequest& NewRequest) override;
	//~End of UU0NumberPopComponent interface

protected:
	
	TArray<int32> DamageNumberArray;

	/** Style patterns to attempt to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UU0DamagePopStyleNiagara> Style;

	//Niagara Component used to display the damage
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
};
