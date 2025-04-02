// Copyright Epic Games, Inc. All Rights Reserved.

#include "U0FrontendStateComponent.h"

#include "CommonGameInstance.h"
#include "CommonSessionSubsystem.h"
#include "ControlFlowManager.h"
#include "U0Game/GameModes/U0ExperienceManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NativeGameplayTags.h"
#include "PrimaryGameLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0FrontendStateComponent)

namespace FrontendTags
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_UI_LAYER_MENU, "UI.Layer.Menu");
}

UU0FrontendStateComponent::UU0FrontendStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UU0FrontendStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for the experience load to complete
	AGameStateBase* GameState = GetGameStateChecked<AGameStateBase>();
	UU0ExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UU0ExperienceManagerComponent>();
	check(ExperienceComponent);

	// This delegate is on a component with the same lifetime as this one, so no need to unhook it in 
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_HighPriority(FOnU0ExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void UU0FrontendStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool UU0FrontendStateComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (bShouldShowLoadingScreen)
	{
		OutReason = TEXT("Frontend Flow Pending...");
		
		if (FrontEndFlow.IsValid())
		{
			const TOptional<FString> StepDebugName = FrontEndFlow->GetCurrentStepDebugName();
			if (StepDebugName.IsSet())
			{
				OutReason = StepDebugName.GetValue();
			}
		}
		
		return true;
	}

	return false;
}

void UU0FrontendStateComponent::OnExperienceLoaded(const UU0ExperienceDefinition* Experience)
{
	FControlFlow& Flow = FControlFlowStatics::Create(this, TEXT("FrontendFlow"))
		.QueueStep(TEXT("Wait For User Initialization"), this, &ThisClass::FlowStep_WaitForUserInitialization)
		.QueueStep(TEXT("Try Show Press Start Screen"), this, &ThisClass::FlowStep_TryShowPressStartScreen);

	Flow.ExecuteFlow();

	FrontEndFlow = Flow.AsShared();
}

void UU0FrontendStateComponent::FlowStep_WaitForUserInitialization(FControlFlowNodeRef SubFlow)
{
	// If this was a hard disconnect, explicitly destroy all user and session state
	// TODO: Refactor the engine disconnect flow so it is more explicit about why it happened
	bool bWasHardDisconnect = false;
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AGameModeBase>();
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);

	if (ensure(GameMode) && UGameplayStatics::HasOption(GameMode->OptionsString, TEXT("closed")))
	{
		bWasHardDisconnect = true;
	}

	// Always reset sessions

	SubFlow->ContinueFlow();
}

void UU0FrontendStateComponent::FlowStep_TryShowPressStartScreen(FControlFlowNodeRef SubFlow)
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);

	// Check to see if the platform actually requires a 'Press Start' screen.  This is only
	// required on platforms where there can be multiple online users where depending on what player's
	// controller presses 'Start' establishes the player to actually login to the game with.

	// Add the Press Start screen, move to the next flow when it deactivates.
	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this))
	{
		constexpr bool bSuspendInputUntilComplete = true;
		RootLayout->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(FrontendTags::TAG_UI_LAYER_MENU, bSuspendInputUntilComplete, PressStartScreenClass,
			[this, SubFlow](EAsyncWidgetLayerState State, UCommonActivatableWidget* Screen) {
			switch (State)
			{
			case EAsyncWidgetLayerState::AfterPush:
				bShouldShowLoadingScreen = false;
				Screen->OnDeactivated().AddWeakLambda(this, [this, SubFlow]() {
					SubFlow->ContinueFlow();
				});
				break;
			case EAsyncWidgetLayerState::Canceled:
				bShouldShowLoadingScreen = false;
				SubFlow->ContinueFlow();
				return;
			}
		});
	}
}

void UU0FrontendStateComponent::FlowStep_TryJoinRequestedSession(FControlFlowNodeRef SubFlow)
{
	UCommonGameInstance* GameInstance = Cast<UCommonGameInstance>(UGameplayStatics::GetGameInstance(this));
	
	// Skip this step if we didn't start requesting a session join
	SubFlow->ContinueFlow();
}

void UU0FrontendStateComponent::FlowStep_TryShowMainScreen(FControlFlowNodeRef SubFlow)
{
	if (UPrimaryGameLayout* RootLayout = UPrimaryGameLayout::GetPrimaryGameLayoutForPrimaryPlayer(this))
	{
		constexpr bool bSuspendInputUntilComplete = true;
		RootLayout->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(FrontendTags::TAG_UI_LAYER_MENU, bSuspendInputUntilComplete, MainScreenClass,
			[this, SubFlow](EAsyncWidgetLayerState State, UCommonActivatableWidget* Screen) {
			switch (State)
			{
			case EAsyncWidgetLayerState::AfterPush:
				bShouldShowLoadingScreen = false;
				SubFlow->ContinueFlow();
				return;
			case EAsyncWidgetLayerState::Canceled:
				bShouldShowLoadingScreen = false;
				SubFlow->ContinueFlow();
				return;
			}
		});
	}
}
