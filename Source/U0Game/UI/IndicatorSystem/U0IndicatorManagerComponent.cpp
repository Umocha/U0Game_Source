// Copyright Epic Games, Inc. All Rights Reserved.

#include "U0IndicatorManagerComponent.h"

#include "IndicatorDescriptor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0IndicatorManagerComponent)

UU0IndicatorManagerComponent::UU0IndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
}

/*static*/ UU0IndicatorManagerComponent* UU0IndicatorManagerComponent::GetComponent(AController* Controller)
{
	if (Controller)
	{
		return Controller->FindComponentByClass<UU0IndicatorManagerComponent>();
	}

	return nullptr;
}

void UU0IndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	IndicatorDescriptor->SetIndicatorManagerComponent(this);
	OnIndicatorAdded.Broadcast(IndicatorDescriptor);
	Indicators.Add(IndicatorDescriptor);
}

void UU0IndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	if (IndicatorDescriptor)
	{
		ensure(IndicatorDescriptor->GetIndicatorManagerComponent() == this);
	
		OnIndicatorRemoved.Broadcast(IndicatorDescriptor);
		Indicators.Remove(IndicatorDescriptor);
	}
}
