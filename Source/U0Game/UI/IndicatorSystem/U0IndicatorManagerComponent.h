// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ControllerComponent.h"

#include "U0IndicatorManagerComponent.generated.h"

class AController;
class UIndicatorDescriptor;
class UObject;
struct FFrame;

/**
 * @class UU0IndicatorManagerComponent
 */
UCLASS(BlueprintType, Blueprintable)
class U0GAME_API UU0IndicatorManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UU0IndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);

	static UU0IndicatorManagerComponent* GetComponent(AController* Controller);

	UFUNCTION(BlueprintCallable, Category = Indicator)
	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);
	
	UFUNCTION(BlueprintCallable, Category = Indicator)
	void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	DECLARE_EVENT_OneParam(UU0IndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* Descriptor)
	FIndicatorEvent OnIndicatorAdded;
	FIndicatorEvent OnIndicatorRemoved;

	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }

private:
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
};
