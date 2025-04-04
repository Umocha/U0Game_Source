// Fill out your copyright notice in the Description page of Project Settings.


#include "U0PartyManagementComponent.h"
#include "U0Game/Character/U0CharacterData.h"
#include "U0Game/Character/U0Character.h"
#include "U0Game/Character/U0PawnExtensionComponent.h"
#include "U0Game/Player/U0PlayerState.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"
#include "U0Game/Character/U0HeroComponent.h"
#include "U0Game/Player/U0PlayerController.h"
#include "U0Game/Input/U0InputComponent.h"
#include "InputAction.h"
#include "U0Game/AbilitySystem/U0AbilitySet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "U0Game/U0GameplayTags.h"
#include "U0Game/AbilitySystem/Attributes/U0HeroSkillSetBase.h"
#include "U0Game/Character/U0HealthComponent.h"
#include "U0Game/Messages/U0VerbMessage.h"
#include "U0Game/Messages/U0VerbMessageHelpers.h"

#include "GameFramework/GameplayMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0PartyManagementComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_U0_PartyBar_Message_SlotsChanged, "U0.PartyBar.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_U0_PartyBar_Message_ActiveIndexChanged, "U0.PartyBar.Message.ActiveIndexChanged");

//////////////////////////////////////
// FPartyEntry
//////////////////////////////////////
void FPartyEntry::SetActivate(bool bIsActivate)
{
	bActivate = bIsActivate;
	SpawnedPawn->SetActorEnableCollision(bIsActivate);
	SpawnedPawn->SetActorHiddenInGame(!bIsActivate);
}

//////////////////////////////////////
// UU0PartyManagementComponent
//////////////////////////////////////
UU0PartyManagementComponent::UU0PartyManagementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UU0PartyManagementComponent::BeginPlay()
{
	if (AU0PlayerState* U0PS = GetPlayerState<AU0PlayerState>())
	{
		NumSlots = U0PS->GetNumPartySlots();
		U0PlayerState = U0PS;
		U0AbilitySystemComponent = U0PS->GetU0AbilitySystemComponent();
		if (AU0PlayerController* U0PC = CastChecked<AU0PlayerController>(U0PS->GetPlayerController()))
		{
			U0PC->SetU0PartyManagementComponent(this);
			U0PlayerController = U0PC;
		}
	}
	Super::BeginPlay();
}

void UU0PartyManagementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (FPartyEntry& Entry : PartyEntries)
	{
		DestroyEntry(Entry);
	}

	Super::EndPlay(EndPlayReason);
}

void UU0PartyManagementComponent::InitializePartyList()
{
	if (GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint) || !GetClass()->HasAnyClassFlags(CLASS_Native))
	{
		ReceiveInitializePartyList();
	}
	
}

void UU0PartyManagementComponent::SetPartyList(TArray<const UU0CharacterData*> NewPartyList)
{
	if (NewPartyList.Num() == 0)
	{
		return;
	}

	TArray<FPartyEntry> RemoveEntries = PartyEntries;
	PartyEntries.Reset();
	PartyEntries.AddDefaulted(NumSlots);


	// 기본 Spawn 위치값
	FTransform PawnTransform;
	if (AU0PlayerController* U0PC = U0PlayerController.Get())
	{
		if (APawn* Pawn = U0PC->GetPawn())
		{
			PawnTransform = Pawn->GetTransform();
		}
		else if (AActor* StartSpot = U0PC->StartSpot.Get())
		{
			PawnTransform = StartSpot->GetTransform();
		}
	}

	// 현재 List와 비교해서 같은 캐릭터가 있거나 위치만 바꿔야 할 경우 최적화
	for (int32 SlotIndex = 0; SlotIndex < NewPartyList.Num(); SlotIndex++)
	{
		for (int32 RemoveSlotIndex = 0; RemoveSlotIndex < RemoveEntries.Num(); RemoveSlotIndex++)
		{
			if (NewPartyList[SlotIndex] == RemoveEntries[RemoveSlotIndex].CharacterData)
			{
				if (SlotIndex == RemoveSlotIndex)
				{
					PartyEntries[SlotIndex] = RemoveEntries[RemoveSlotIndex];
				}
				else 
				{
					SetSlotEntry(RemoveEntries[RemoveSlotIndex], SlotIndex);
					if (RemoveEntries[RemoveSlotIndex].bActivate)
					{
						ActiveSlotIndex = SlotIndex;
						U0AbilitySystemComponent->ActivateAbilityInSlot(SlotIndex);

					}
				}
				RemoveEntries[RemoveSlotIndex] = FPartyEntry();
				break;
			}
		}

		if (PartyEntries[SlotIndex].SpawnedPawn == nullptr)
		{
			FPartyEntry NewEntry = SpawnEntry(NewPartyList[SlotIndex]);
			SetSlotEntry(NewEntry, SlotIndex);
		}
		
	}

	OnRep_Entries();

	// Despawn되야할 CharacterList
	for (FPartyEntry& RemoveEntry : RemoveEntries)
	{
		DestroyEntry(RemoveEntry);
	}

	// 죽어있는 캐릭터가 Party에 지정된 경우
	if (UU0HealthComponent* HealthComponent = UU0HealthComponent::FindHealthComponent(PartyEntries[0].SpawnedPawn))
	{
		if (HealthComponent->GetDeathState() != EU0DeathState::NotDead)
		{
			OnDeathFinished(PartyEntries[0].SpawnedPawn);
			return;
		}
	}

	// 캐릭터 Activate
	if (PartyEntries[ActiveSlotIndex].SpawnedPawn != nullptr)
	{
		ActivateIndex(ActiveSlotIndex);
	}
	else
	{
		SetActivePartyIndex(0);
	}

	PartyEntries[ActiveSlotIndex].SpawnedPawn->SetActorTransform(PawnTransform);
}

FPartyEntry UU0PartyManagementComponent::SpawnEntry(const UU0CharacterData* CharacterData)
{
	FPartyEntry NewEntry;
	NewEntry.CharacterData = CharacterData;

	FActorSpawnParameters SpawnInfo;
	// SpawnInfo.Instigator
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	FTransform SpawnTransform;
	if (AU0Character* SpawnedPawn = GetWorld()->SpawnActor<AU0Character>(CharacterData->PawnClass, SpawnTransform, SpawnInfo))
	{
		// Spawn 해주면서 별도 초기화 작업
		if (UU0PawnExtensionComponent* PawnExtComp = UU0PawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
		{
			PawnExtComp->SetPawnData(CharacterData);
		}

		// 캐릭터가 죽을 경우 Event Bind
		SpawnedPawn->HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
		SpawnedPawn->HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

		SpawnTransform.SetLocation(FVector(100.f, 100.f, 100.f));
		SpawnedPawn->FinishSpawning(SpawnTransform);
		NewEntry.SpawnedPawn = SpawnedPawn;
		NewEntry.SetActivate(false);
	}

	return NewEntry;
}

void UU0PartyManagementComponent::DestroyEntry(FPartyEntry& Entry)
{
	if (Entry.SpawnedPawn != nullptr)
	{
		Entry.SpawnedPawn->Destroy();
		Entry.SpawnedPawn = nullptr;
	}
}

void UU0PartyManagementComponent::SetSlotEntry(FPartyEntry& Entry, int32 Index)
{
	// 유효 체크
	if (Entry.SpawnedPawn == nullptr)
	{
		return;
	}
	PartyEntries[Index] = Entry;

	// 캐릭터에도 지정된 SlotNum 넘겨준다.
	if (AU0Character* Character = Cast<AU0Character>(Entry.SpawnedPawn))
	{
		Character->SetSlotIndex(Index);
	}


	// Slot에 캐릭터가 Set되면 기존 Slot Ability Clear 후, 캐릭터가 가지고 있는 AbilitySet을 통해 GiveAbility
	if(UU0AbilitySystemComponent* U0ASC = U0AbilitySystemComponent.Get())
	{
		U0ASC->ClearAllAbilitiesWithInputID(Index);
	
		if (UU0PawnExtensionComponent* PawnExtComp = UU0PawnExtensionComponent::FindPawnExtensionComponent(Entry.SpawnedPawn))
		{
			PawnExtComp->InitializeAbilitySystem(U0ASC, GetOwner());
		}

		for (UU0AbilitySet* AbilitySet : Entry.CharacterData->AbilitySets)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveToAbilitySystem(U0ASC, nullptr, nullptr, Index);
			}
		}
	}
}


void UU0PartyManagementComponent::SetActivePartyIndex(int32 NewIndex, bool IgnoreOutro)
{
	if (!CanSwitch(NewIndex))
	{
		return;
	}
	
	FVector ActorLocaion = FVector::ZeroVector;
	FRotator ActorRotation;
	FVector CurrentVelocity;
	EMovementMode CurMovementMode = EMovementMode::MOVE_Walking;
	UAnimMontage* CurrentMontage = nullptr;
	int32 OldIndex = ActiveSlotIndex;

	// 캐릭터 퇴장(Outro) 로직
	if (PartyEntries[OldIndex].SpawnedPawn != nullptr)
	{
		ActorLocaion = PartyEntries[OldIndex].SpawnedPawn->GetActorLocation();
		ActorRotation = PartyEntries[OldIndex].SpawnedPawn->GetActorRotation();
		UCharacterMovementComponent* MovementComponent = PartyEntries[OldIndex].SpawnedPawn->GetCharacterMovement();
		if (MovementComponent)
		{
			CurrentVelocity = MovementComponent->Velocity;
			CurMovementMode = MovementComponent->MovementMode;
		}
		if (PartyEntries[OldIndex].bActivate && !IgnoreOutro)
		{
			OutroActiveCharacter();
		}


		UAnimInstance* AnimInstance = PartyEntries[OldIndex].SpawnedPawn->GetMesh()->GetAnimInstance();
		CurrentMontage = AnimInstance->GetCurrentActiveMontage();
		
		if (CurrentMontage && (CurrentMontage->GetSectionIndex(AnimInstance->Montage_GetCurrentSection()) > CurrentMontage->GetSectionIndex("Loop")))
		{
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
			// 바인딩할 Delegate 생성
			FOnMontageEnded MontageEndDelegate;
			MontageEndDelegate.BindLambda([this, OldIndex](UAnimMontage* Montage, bool bInterrupted)
				{
					// Montage가 끝났으니 이제 퇴장
					if (this->ActiveSlotIndex != OldIndex)
					{
						DeactivateIndex(OldIndex);
					}
				});

			AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, CurrentMontage);
			//PartyEntries[OldIndex].SpawnedPawn->SetActorEnableCollision(false);
		}
		else
		{
			AnimInstance->Montage_Stop(0);
			// 바로 퇴장
			DeactivateIndex(OldIndex);
		}
		
	}
		
	ActiveSlotIndex = NewIndex;

	// 캐릭터 등장(Intro) 로직
	if (PartyEntries[ActiveSlotIndex].SpawnedPawn != nullptr)
	{
		PartyEntries[NewIndex].SpawnedPawn->SetActorLocation(ActorLocaion);
		PartyEntries[NewIndex].SpawnedPawn->SetActorRotation(ActorRotation, ETeleportType::TeleportPhysics);
		ActivateIndex(ActiveSlotIndex);

		if (UU0AbilitySystemComponent* U0ASC = U0AbilitySystemComponent.Get())
		{
			if (U0ASC->HasMatchingGameplayTag(U0GameplayTags::Status_InCombat))
			{
				IntroActiveCharacter();
			}
			else
			{
				if (UCharacterMovementComponent* MovementComponent = PartyEntries[NewIndex].SpawnedPawn->GetCharacterMovement())
				{
					MovementComponent->SetMovementMode(CurMovementMode);
					MovementComponent->Velocity = CurrentVelocity;
				}
			}
		}
	}

}

bool UU0PartyManagementComponent::CanSwitch(int32 Index)
{
	if (Index < 0 || !PartyEntries.IsValidIndex(Index))
	{
		return false;
	}

	if (PartyEntries[Index].SpawnedPawn == nullptr)
	{
		return false;
	}

	if (ActiveSlotIndex == Index)
	{
		return false;
	}

	if (UU0HealthComponent* HealthComponent = UU0HealthComponent::FindHealthComponent(PartyEntries[Index].SpawnedPawn))
	{
		if (HealthComponent->GetDeathState() != EU0DeathState::NotDead)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

void UU0PartyManagementComponent::RespawnAtPosition(FVector SpawnPosition)
{
	PartyEntries[0].SpawnedPawn->SetActorLocation(SpawnPosition);
	ActivateIndex(0);
	if (UU0AbilitySystemComponent* U0ASC = U0AbilitySystemComponent.Get())
	{
		U0ASC->RemoveLooseGameplayTag(U0GameplayTags::Status_Death_Dead);
	}
}

bool UU0PartyManagementComponent::ReviveSlot(int32 Index)
{
	if (Index < 0 || !PartyEntries.IsValidIndex(Index))
	{
		return false;
	}

	if (PartyEntries[Index].SpawnedPawn == nullptr)
	{
		return false;
	}

	if (UU0HealthComponent* HealthComp = UU0HealthComponent::FindHealthComponent(PartyEntries[Index].SpawnedPawn))
	{
		if (HealthComp->IsDeadOrDying())
		{
			HealthComp->Revive();

			return true;
		}
	}

	return false;
}

void UU0PartyManagementComponent::ActivateIndex(int32 Index)
{
	if (PartyEntries[Index].SpawnedPawn == nullptr)
	{
		return;
	}

	PartyEntries[Index].SetActivate(true);
	ActiveSlotIndex = Index;

	U0PlayerController->Possess(PartyEntries[Index].SpawnedPawn);
	U0AbilitySystemComponent->InitAbilityActorInfo(U0PlayerState.Get(), PartyEntries[Index].SpawnedPawn);
	U0AbilitySystemComponent->ActivateAbilityInSlot(Index);

	OnRep_ActiveSlotIndex();
}

void UU0PartyManagementComponent::DeactivateIndex(int32 Index)
{
	if (PartyEntries[Index].SpawnedPawn == nullptr)
	{
		return;
	}
		
	if (UCharacterMovementComponent* MovementComponent = PartyEntries[Index].SpawnedPawn->GetCharacterMovement())
	{
		MovementComponent->SetMovementMode(EMovementMode::MOVE_None);
		MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	PartyEntries[Index].SetActivate(false);
}


void UU0PartyManagementComponent::OutroActiveCharacter()
{
	if (UU0AbilitySystemComponent* U0ASC = U0AbilitySystemComponent.Get())
	{
		U0ASC->RemoveLooseGameplayTag(U0GameplayTags::GameplayEvent_Intro_Possibility);

		// 전투 중인지 확인
		if (U0ASC->HasMatchingGameplayTag(U0GameplayTags::Status_InCombat))
		{
			const UU0HeroSkillSetBase* SkillSet = U0ASC->GetSkillSet(ActiveSlotIndex);
			// 협주 게이지 확인
			if (SkillSet->GetConcertoEnergy() == SkillSet->GetMaxConcertoEnergy())
			{
				U0ASC->SetNumericAttributeBase(SkillSet->GetConcertoEnergyAttribute(), 0);
				U0ASC->AddLooseGameplayTag(U0GameplayTags::GameplayEvent_Intro_Possibility);
			}
		}
	}
}

void UU0PartyManagementComponent::IntroActiveCharacter()
{
	if (UU0AbilitySystemComponent* U0ASC = U0AbilitySystemComponent.Get())
	{
		if (U0ASC->HasMatchingGameplayTag(U0GameplayTags::Status_InCombat))
		{
			FGameplayEventData Payload;
			U0ASC->HandleGameplayEvent(U0GameplayTags::GameplayEvent_Intro, &Payload);
		}
	}
}

void UU0PartyManagementComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	if (PartyEntries[ActiveSlotIndex].SpawnedPawn)
	{
		if (UU0HeroComponent* U0HeroComp = PartyEntries[ActiveSlotIndex].SpawnedPawn->FindComponentByClass<UU0HeroComponent>())
		{
			U0HeroComp->Input_Move(InputActionValue);
		}
	}
}

void UU0PartyManagementComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	if (PartyEntries[ActiveSlotIndex].SpawnedPawn)
	{
		if (UU0HeroComponent* U0HeroComp = PartyEntries[ActiveSlotIndex].SpawnedPawn->FindComponentByClass<UU0HeroComponent>())
		{
			U0HeroComp->Input_LookMouse(InputActionValue);
		}
	}

}

void UU0PartyManagementComponent::OnRep_Entries()
{
	FU0PartyBarSlotsChangedMessage Message;
	Message.Owner = U0PlayerController.Get();
	Message.PartyEntries = PartyEntries;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_U0_PartyBar_Message_SlotsChanged, Message);
}

void UU0PartyManagementComponent::OnRep_ActiveSlotIndex()
{
	FU0PartyBarActiveIndexChangedMessage Message;
	Message.Owner = U0PlayerController.Get();
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_U0_PartyBar_Message_ActiveIndexChanged, Message);
}

void UU0PartyManagementComponent::OnDeathStarted(AActor* OwningActor)
{
	
}

void UU0PartyManagementComponent::OnDeathFinished(AActor* OwningActor)
{
	for (int NewSlot = 0; NewSlot < NumSlots; NewSlot++)
	{
		if (CanSwitch(NewSlot))
		{
			SetActivePartyIndex(NewSlot, true);
			return;
		}
	}
	
	// Team 전멸 Respawn UI
	DeactivateIndex(ActiveSlotIndex);
	if (UU0AbilitySystemComponent* U0ASC = U0AbilitySystemComponent.Get())
	{
		U0ASC->AddLooseGameplayTag(U0GameplayTags::Status_Death_Dead);
		FU0RequestRespawnMessage Message;
		Message.RequesterAbilitySystemComponent = U0ASC;

		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
		MessageSystem.BroadcastMessage(U0GameplayTags::GameplayEvent_RequestRespawn, Message);
	}
}
