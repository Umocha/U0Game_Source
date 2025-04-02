// Fill out your copyright notice in the Description page of Project Settings.


#include "U0PlayerController.h"
#include "U0Game/Camera/U0PlayerCameraManager.h"
#include "U0PlayerState.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"
#include "U0Game/Input/U0InputComponent.h"
#include "U0Game/U0GameplayTags.h"
#include "U0Game/Character/U0PawnData.h"
#include "U0Game/CharacterManagement/U0PartyManagementComponent.h"
#include "U0Game/Camera/U0CameraMode_ThirdPerson.h"
#include "U0Game/Camera/U0CameraComponent.h"
#include "Interfaces/NetworkPredictionInterface.h"
#include "GameFramework/PawnMovementComponent.h"

#include "U0Game/U0LogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0PlayerController)

AU0PlayerController::AU0PlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{  
	PlayerCameraManagerClass = AU0PlayerCameraManager::StaticClass();
}

void AU0PlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	// 우선 PostProcessInput()이 언제 호출되는지 확인해보자:
	// - UPlayerInput::ProcessInputStack()에서 호출된다

	if (UU0AbilitySystemComponent* U0ASC = GetU0AbilitySystemComponent())
	{
		U0ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AU0PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

}

void AU0PlayerController::SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TransitionParams.BlendTime = 0.2f;
	TransitionParams.BlendFunction = EViewTargetBlendFunction::VTBlend_EaseOut;
	TransitionParams.bLockOutgoing = true;

	Super::SetViewTarget(NewViewTarget, TransitionParams);
}

void AU0PlayerController::OnPossess(APawn* PawnToPossess)
{
	if (PawnToPossess != NULL &&
		(PlayerState == NULL || !PlayerState->IsOnlyASpectator()))
	{
		const bool bNewPawn = (GetPawn() != PawnToPossess);

		if (GetPawn() && bNewPawn)
		{
			UnPossess();
		}

		if (PawnToPossess->Controller != NULL)
		{
			PawnToPossess->Controller->UnPossess();
		}

		PawnToPossess->PossessedBy(this);

		// update rotation to match possessed pawn's rotation
		// SetControlRotation(PawnToPossess->GetActorRotation());

		SetPawn(PawnToPossess);
		check(GetPawn() != NULL);

		if (GetPawn() && GetPawn()->PrimaryActorTick.bStartWithTickEnabled)
		{
			GetPawn()->SetActorTickEnabled(true);
		}

		INetworkPredictionInterface* NetworkPredictionInterface = GetPawn() ? Cast<INetworkPredictionInterface>(GetPawn()->GetMovementComponent()) : NULL;
		if (NetworkPredictionInterface)
		{
			NetworkPredictionInterface->ResetPredictionData_Server();
		}

		AcknowledgedPawn = NULL;

		// Local PCs will have the Restart() triggered right away in ClientRestart (via PawnClientRestart()), but the server should call Restart() locally for remote PCs.
		// We're really just trying to avoid calling Restart() multiple times.
		if (!IsLocalPlayerController())
		{
			GetPawn()->DispatchRestart(false);
		}

		ClientRestart(GetPawn());

		ChangeState(NAME_Playing);
		if (bAutoManageActiveCameraTarget)
		{
			AutoManageActiveCameraTarget(GetPawn());
			ResetCameraMode();
		}
	}
}

void AU0PlayerController::InitializePlayerInput(const UU0InputConfig* InputConfig)
{
	UU0InputComponent* U0IC = CastChecked<UU0InputComponent>(InputComponent);
	if (U0IC)
	{

		TArray<uint32> BindHandles;
		U0IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);

		// InputTag_Move와 InputTag_Look_Mouse에 대해 각각 Input_Move()와 Input_LookMouse() 멤버 함수에 바인딩 시킨다:
		// - 바인딩한 이후, Input 이벤트에 따라 멤버 함수가 트리거된다
		U0IC->BindNativeAction(InputConfig, U0GameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
		U0IC->BindNativeAction(InputConfig, U0GameplayTags::InputTag_Move, ETriggerEvent::Completed, this, &ThisClass::Release_Move, false);
		U0IC->BindNativeAction(InputConfig, U0GameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
		U0IC->BindNativeAction(InputConfig, U0GameplayTags::InputTag_PartySlot_SelectSlot, ETriggerEvent::Triggered, this, &ThisClass::Input_SelectPartySlot, false);
		U0IC->BindNativeAction(InputConfig, U0GameplayTags::InputTag_PartySlot_SelectSlot, ETriggerEvent::Completed, this, &ThisClass::Release_SelectPartySlot, false);
		U0IC->BindNativeAction(InputConfig, U0GameplayTags::InputTag_ZoomInOut_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_ZoomInOutMouse, false);
		U0IC->BindNativeAction(InputConfig, U0GameplayTags::InputTag_Cursor_Mouse_Press, ETriggerEvent::Started, this, &ThisClass::ShowMouseCursor, false);
		U0IC->BindNativeAction(InputConfig, U0GameplayTags::InputTag_Cursor_Mouse_Release, ETriggerEvent::Triggered, this, &ThisClass::HideMouseCursor, false);
	}
}

void AU0PlayerController::Input_Move(const FInputActionValue& InputActionValue)
{
	if (UU0AbilitySystemComponent* U0ASC = GetU0AbilitySystemComponent())
	{
		if (U0ASC->HasMatchingGameplayTag(U0GameplayTags::Status_Knockback))
		{
			return;
		}
	}
	if (U0PartyManagementComponent.IsValid())
	{
		U0PartyManagementComponent->Input_Move(InputActionValue);
	}

	InputDir = InputActionValue.Get<FVector2D>();
}

void AU0PlayerController::Release_Move(const FInputActionValue& InputActionValue)
{
	InputDir = FVector2D::Zero();
}

void AU0PlayerController::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	if (U0PartyManagementComponent.IsValid())
	{
		U0PartyManagementComponent->Input_LookMouse(InputActionValue);
	}
}

void AU0PlayerController::Input_SelectPartySlot(const FInputActionValue& InputActionValue)
{
	if (UU0AbilitySystemComponent* U0ASC = GetU0AbilitySystemComponent())
	{
		int32 SelectNum = InputActionValue.Get<float>();
		if (U0PartyManagementComponent.IsValid())
		{
			if (!U0PartyManagementComponent->CanSwitch(SelectNum - 1))
			{
				return;
			}
		}

		switch (SelectNum)
		{
		case 1:
			U0ASC->AbilityInputTagPressed(U0GameplayTags::InputTag_Ability_PartySlot_Select_Slot1);
			return;
		case 2:
			U0ASC->AbilityInputTagPressed(U0GameplayTags::InputTag_Ability_PartySlot_Select_Slot2);
			return;
		case 3:
			U0ASC->AbilityInputTagPressed(U0GameplayTags::InputTag_Ability_PartySlot_Select_Slot3);
			return;
		default:
			return;
		}
	}
}

void AU0PlayerController::Release_SelectPartySlot(const FInputActionValue& InputActionValue)
{
	if (UU0AbilitySystemComponent* U0ASC = GetU0AbilitySystemComponent())
	{
		U0ASC->AbilityInputTagReleased(U0GameplayTags::InputTag_Ability_PartySlot_Select_Slot1);
		U0ASC->AbilityInputTagReleased(U0GameplayTags::InputTag_Ability_PartySlot_Select_Slot2);
		U0ASC->AbilityInputTagReleased(U0GameplayTags::InputTag_Ability_PartySlot_Select_Slot3);
	}
}

void AU0PlayerController::Input_ZoomInOutMouse(const FInputActionValue& InputActionValue)
{
	const float Value = InputActionValue.Get<float>();
	
	UU0CameraMode_ThirdPerson::HandleCameraZoomInOut(Value);
}

void AU0PlayerController::ShowMouseCursor()
{
	if (!bShowMouseCursor)
	{
		bShowMouseCursor = true;
		SetInputMode(FInputModeGameAndUI());
		UE_LOG(LogU0, Display, TEXT("Triggered"));
	}
}

void AU0PlayerController::HideMouseCursor()
{
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
	UE_LOG(LogU0, Display, TEXT("Completed"));
}

void AU0PlayerController::SelectPartySlot(const int32 SelectNum)
{
	if (U0PartyManagementComponent.IsValid())
	{
		U0PartyManagementComponent->SetActivePartyIndex(SelectNum - 1);
	}
}

void AU0PlayerController::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UU0AbilitySystemComponent* U0ASC = GetU0AbilitySystemComponent())
	{
			U0ASC->AbilityInputTagPressed(InputTag);
	}
}

void AU0PlayerController::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UU0AbilitySystemComponent* U0ASC = GetU0AbilitySystemComponent())
	{
		U0ASC->AbilityInputTagReleased(InputTag);
	}
}

void AU0PlayerController::SetU0PartyManagementComponent(UU0PartyManagementComponent* InU0PartyManagementComponent)
{
	if (U0PartyManagementComponent.IsValid())
	{
		return;
	}
	U0PartyManagementComponent = InU0PartyManagementComponent;
}


AU0PlayerState* AU0PlayerController::GetU0PlayerState() const
{
	return CastChecked<AU0PlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UU0AbilitySystemComponent* AU0PlayerController::GetU0AbilitySystemComponent() const
{
	if (const AU0PlayerState* U0PS = GetU0PlayerState())
	{
		return U0PS->GetU0AbilitySystemComponent();
	}

	return nullptr;
}
