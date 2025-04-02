// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameStateComponent.h"
#include "ControlFlowNode.h"
#include "LoadingProcessInterface.h"

#include "U0FrontendStateComponent.generated.h"

class FControlFlow;
class FString;
class FText;
class UObject;
struct FFrame;

class UCommonActivatableWidget;
class UU0ExperienceDefinition;

UCLASS(Abstract)
class UU0FrontendStateComponent : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:

	UU0FrontendStateComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	//~ILoadingProcessInterface interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~End of ILoadingProcessInterface

private:
	void OnExperienceLoaded(const UU0ExperienceDefinition* Experience);

	void FlowStep_WaitForUserInitialization(FControlFlowNodeRef SubFlow);
	void FlowStep_TryShowPressStartScreen(FControlFlowNodeRef SubFlow);
	void FlowStep_TryJoinRequestedSession(FControlFlowNodeRef SubFlow);
	void FlowStep_TryShowMainScreen(FControlFlowNodeRef SubFlow);

	bool bShouldShowLoadingScreen = true;

	UPROPERTY(EditAnywhere, Category = UI)
	TSoftClassPtr<UCommonActivatableWidget> PressStartScreenClass;

	UPROPERTY(EditAnywhere, Category = UI)
	TSoftClassPtr<UCommonActivatableWidget> MainScreenClass;

	TSharedPtr<FControlFlow> FrontEndFlow;
	
	// If set, this is the in-progress press start screen task
	FControlFlowNodePtr InProgressPressStartScreen;

	FDelegateHandle OnJoinSessionCompleteEventHandle;
};
