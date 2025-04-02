// Fill out your copyright notice in the Description page of Project Settings.


#include "U0HeroComponent.h"
#include "U0PawnExtensionComponent.h"
#include "U0PawnData.h"
#include "Components/GameFrameworkComponentManager.h"
#include "PlayerMappableInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "U0Game/U0LogChannels.h"
#include "U0Game/U0GameplayTags.h"
#include "U0Game/Player/U0PlayerState.h"
#include "U0Game/Camera/U0CameraComponent.h"
#include "U0Game/Player/U0PlayerController.h"
#include "U0Game/Input/U0InputComponent.h"
#include "U0Game/Input/U0InputConfig.h"
#include "U0Game/Input/U0MappableConfigPair.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(U0HeroComponent)

const FName UU0HeroComponent::NAME_ActorFeatureName("Hero");
/* InputConfig의 GameFeatureAction 활성화 ExtensionEvent 이름 */
const FName UU0HeroComponent::NAME_BindInputsNow("BindInputsNow");

UU0HeroComponent::UU0HeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReadyToBindInputs = false;
}

void UU0HeroComponent::OnRegister()
{
	Super::OnRegister();

	// 올바론 Actor에 등록되었는지 확인:
	{
		if (!GetPawn<APawn>())
		{
			UE_LOG(LogU0, Error, TEXT("this component has been added to BP whose base class is not a Pawn!"));
			return;
		}
	}

	RegisterInitStateFeature();
}

UE_DISABLE_OPTIMIZATION

void UU0HeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// PawnExtsionComponent에 대해서 (PawnExtension Feature) OnActorInitStateChanged() 관찰하도록 (Observing)
	BindOnActorInitStateChanged(UU0PawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// InitState_Spawned로 초기화
	ensure(TryToChangeInitState(U0GameplayTags::InitState_Spawned));

	// ForceUpdate 진행
	CheckDefaultInitialization();
}
UE_ENABLE_OPTIMIZATION

void UU0HeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UU0HeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UU0PawnExtensionComponent::NAME_ActorFeatureName)
	{
		// U0PawnExtensionComponent의 DataInitialized 상태 변화 관찰 후, U0HeroComponent도 DataInitialized 상태로 변경
		// - CanChangeInitState 확인
		if (Params.FeatureState == U0GameplayTags::InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
	
}

bool UU0HeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	AU0PlayerState* U0PS = GetPlayerState<AU0PlayerState>();

	// Spawned 초기화
	if (!CurrentState.IsValid() && DesiredState == U0GameplayTags::InitState_Spawned)
	{
		// Pawn이 잘 세팅만 되어있다면 바로 Spawned로 넘어감!
		if (Pawn)
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	if (CurrentState == U0GameplayTags::InitState_Spawned && DesiredState == U0GameplayTags::InitState_DataAvailable)
	{
		// 아마 PawnData를 누군가 설정하는 모양이다
		if (!U0PS)
		{
			return false;
		}

		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == U0GameplayTags::InitState_DataAvailable && DesiredState == U0GameplayTags::InitState_DataInitialized)
	{
		// PawnExtensionComponent가 DataInitialized될 때까지 기다림 (== 모든 Feature Component가 DataAvailable인 상태)
		return U0PS && Manager->HasFeatureReachedInitState(Pawn, UU0PawnExtensionComponent::NAME_ActorFeatureName, U0GameplayTags::InitState_DataInitialized);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == U0GameplayTags::InitState_DataInitialized && DesiredState == U0GameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UU0HeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	// DataAvailable -> DataInitialized 단계
	if (CurrentState == U0GameplayTags::InitState_DataAvailable && DesiredState == U0GameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AU0PlayerState* U0PS = GetPlayerState<AU0PlayerState>();
		if (!ensure(Pawn && U0PS))
		{
			return;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UU0PawnData* PawnData = nullptr;
		if (UU0PawnExtensionComponent* PawnExtComp = UU0PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UU0PawnData>();

			// DataInitialized 단계까지 오면, Pawn이 Controller에 Possess되어 준비된 상태이다:
			// - InitAbilityActorInfo 호출로 AvatarActor 재설정이 필요하다
			PawnExtComp->InitializeAbilitySystem(U0PS->GetU0AbilitySystemComponent(), U0PS);
		}

		// Input과 Camera에 대한 핸들링... (TODO)

		// Camera
		if (bIsLocallyControlled && PawnData)
		{
			// 현재 U0Character에 Attach된 CameraComponent를 찾음
			if (UU0CameraComponent* CameraComponent = UU0CameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}

		// Input
		if (AU0PlayerController* U0PC = GetController<AU0PlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
	}
}

void UU0HeroComponent::CheckDefaultInitialization()
{
	// 앞서 BindOnActorInitStateChanged에서 보았듯 Hero Feature는 Pawn Extension Feature에 종속되어 있으므로, CheckDefaultInitializationForImplementers 호출하지 않음:

	// ContinueInitStateChain은 앞서 PawnExtensionComponent와 같음 
	static const TArray<FGameplayTag> StateChain = { U0GameplayTags::InitState_Spawned, U0GameplayTags::InitState_DataAvailable, U0GameplayTags::InitState_DataInitialized, U0GameplayTags::InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

TSubclassOf<UU0CameraMode> UU0HeroComponent::DetermineCameraMode() const
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UU0PawnExtensionComponent* PawnExtComp = UU0PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UU0PawnData* PawnData = PawnExtComp->GetPawnData<UU0PawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

void UU0HeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	// LocalPlayer를 가져오기 위해
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	// EnhancedInputLocalPlayerSubsystem 가져오기 위해
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	// UEnhancedInputLocalPlayerSubsystem에 MappingContext를 비워준다:
	Subsystem->ClearAllMappings();

	// PawnExtensionComponent -> PawnData -> InputConfig 존재 유무 판단:
	if (const UU0PawnExtensionComponent* PawnExtComp = UU0PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UU0PawnData* PawnData = PawnExtComp->GetPawnData<UU0PawnData>())
		{
			if (const UU0InputConfig* InputConfig = PawnData->InputConfig)
			{
				for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				{
					if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
					{
						if (Mapping.bRegisterWithSettings)
						{
							if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(IMC);
							}

							FModifyContextOptions Options = {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							// Actually add the config to the local player							
							Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				UU0InputComponent* U0IC = CastChecked<UU0InputComponent>(PlayerInputComponent);
				if(U0IC)
				{
				
					U0IC->AddInputMappings(InputConfig, Subsystem);
				
					TArray<uint32> BindHandles;
					U0IC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
				
					// InputTag_Move와 InputTag_Look_Mouse에 대해 각각 Input_Move()와 Input_LookMouse() 멤버 함수에 바인딩 시킨다:
					// - 바인딩한 이후, Input 이벤트에 따라 멤버 함수가 트리거된다
					U0IC->BindNativeAction(InputConfig, U0GameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
					U0IC->BindNativeAction(InputConfig, U0GameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
	
	// GameFeatureAction_AddInputConfig의 HandlePawnExtension 콜백 함수 전달
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UU0HeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			// Forward/Backward -> X 값에 들어있음:
			// MovementDirection은 현재 카메라의 RightVector를 의미함 (World-Space)
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);

			// AddMovementInput 함수를 한번 보자:
			// - 내부적으로 MovementDirection * Value.X를 MovementComponent에 적용(더하기)해준다
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			// Left/Right
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UU0HeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		// X에는 Yaw 값이 있음:
		// - Camera에 대해 Yaw 적용
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		// Y = Pitch
		double AimInversionValue = -Value.Y;
		Pawn->AddControllerPitchInput(AimInversionValue);
	}
}

void UU0HeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UU0PawnExtensionComponent* PawnExtComp = UU0PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UU0AbilitySystemComponent* U0ASC = PawnExtComp->GetU0AbilitySystemComponent())
			{
				U0ASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UU0HeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UU0PawnExtensionComponent* PawnExtComp = UU0PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UU0AbilitySystemComponent* U0ASC = PawnExtComp->GetU0AbilitySystemComponent())
			{
				U0ASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

void UU0HeroComponent::AddAdditionalInputConfig(const UU0InputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UU0PawnExtensionComponent* PawnExtComp = UU0PawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UU0InputComponent* LyraIC = Pawn->FindComponentByClass<UU0InputComponent>();
		if (ensureMsgf(LyraIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to ULyraInputComponent or a subclass of it.")))
		{
			LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void UU0HeroComponent::RemoveAdditionalInputConfig(const UU0InputConfig* InputConfig)
{
}

bool UU0HeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}
