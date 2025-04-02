// Fill out your copyright notice in the Description page of Project Settings.


#include "U0AIController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "U0AIPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "U0Game/Character/U0Monster.h"
#include "U0Game/Character/U0PawnData.h"
#include "U0Game/U0GameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0AIController)

AU0AIController::AU0AIController(const FObjectInitializer& ObjectInitializer)
{
	bWantsPlayerState = true;
	bStopAILogicOnUnposses = true;
}

AU0AIPlayerState* AU0AIController::GetU0AIPlayerState() const
{
	return CastChecked<AU0AIPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UU0AbilitySystemComponent* AU0AIController::GetU0AbilitySystemComponent() const
{
	const AU0AIPlayerState* AIPS = GetU0AIPlayerState();
	return (AIPS ? AIPS->GetU0AbilitySystemComponent() : nullptr);
}

void AU0AIController::InitPlayerState()
{
	if (GetNetMode() != NM_Client)
	{
		UWorld* const World = GetWorld();
		const AGameModeBase* GameMode = World ? World->GetAuthGameMode() : NULL;

		if (GameMode != NULL)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = this;
			SpawnInfo.Instigator = GetInstigator();
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.ObjectFlags |= RF_Transient;	// We never want player states to save into a map

			TSubclassOf<APlayerState> PlayerStateClassToSpawn = AU0AIPlayerState::StaticClass();

			PlayerState = World->SpawnActor<APlayerState>(PlayerStateClassToSpawn, SpawnInfo);

			// force a default player name if necessary
			if (PlayerState && PlayerState->GetPlayerName().IsEmpty())
			{
				// don't call SetPlayerName() as that will broadcast entry messages but the GameMode hasn't had a chance
				// to potentially apply a player/bot name yet

				PlayerState->SetPlayerNameInternal(GameMode->DefaultPlayerName.ToString());
			}
		}
	}
}

void AU0AIController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
}

void AU0AIController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void AU0AIController::SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority)
{
	Super::SetFocus(NewFocus, InPriority);

	if (InPriority == EAIFocusPriority::Gameplay)
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(NewFocus))
		{
			ASC->AddLooseGameplayTag(U0GameplayTags::Status_InCombat);
		}
	}
}

void AU0AIController::ClearFocus(EAIFocusPriority::Type InPriority)
{
	if (InPriority == EAIFocusPriority::Gameplay)
	{
		if (AActor* FocusActor = GetFocusActor())
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(FocusActor))
			{
				ASC->RemoveLooseGameplayTag(U0GameplayTags::Status_InCombat);
			}
		}
	}

	Super::ClearFocus(InPriority);
}

void AU0AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AU0Monster* U0Monster = CastChecked<AU0Monster>(InPawn))
	{
		if (UU0AbilitySystemComponent* ASC = GetU0AbilitySystemComponent())
		{
			U0Monster->InitializeAbilitySystem(ASC, PlayerState);

			GetU0AIPlayerState()->SetPawnData(U0Monster->GetPawnData<UU0PawnData>());
		}
	}
}

void AU0AIController::OnUnPossess()
{
	ClearFocus(EAIFocusPriority::Gameplay);

	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}