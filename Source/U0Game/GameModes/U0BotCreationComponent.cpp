// Fill out your copyright notice in the Description page of Project Settings.


#include "U0BotCreationComponent.h"
#include "U0ExperienceManagerComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "U0Game/Character/U0PawnData.h"
#include "U0Game/Character/U0Monster.h"
#include "GameFramework/PlayerState.h"
#include "U0Game/Character/U0HealthComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0BotCreationComponent)


UU0BotCreationComponent::UU0BotCreationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UU0BotCreationComponent::BeginPlay()
{
	Super::BeginPlay();

}

#if WITH_SERVER_CODE
void UU0BotCreationComponent::ServerCreateBots_Implementation()
{
	if (BotControllerClass == nullptr)
	{
		return;
	}

	RemainingBotNames = RandomBotNames;

	// Determine how many bots to spawn
	int32 EffectiveBotCount = NumBotsToCreate;

	// Give the developer settings a chance to override it

	// Create them
	for (int32 Count = 0; Count < EffectiveBotCount; ++Count)
	{
		SpawnOneBot();
	}
}

void UU0BotCreationComponent::OnAIControllerDestroyed(AActor* DestroyedActor)
{
	bAllDie = true;
	for (AAIController* AIController : SpawnedBotList)
	{
		if (AIController && !AIController->IsPendingKillPending())
		{
			bAllDie = false;
		}
	}

	if (bAllDie == true)
	{
		OnAllDead.Broadcast();
	}
}

FString UU0BotCreationComponent::CreateBotName(int32 PlayerIndex)
{
	FString Result;
	if (RemainingBotNames.Num() > 0)
	{
		const int32 NameIndex = FMath::RandRange(0, RemainingBotNames.Num() - 1);
		Result = RemainingBotNames[NameIndex];
		RemainingBotNames.RemoveAtSwap(NameIndex);
	}
	else
	{
		//@TODO: PlayerId is only being initialized for players right now
		PlayerIndex = FMath::RandRange(260, 260 + 100);
		Result = FString::Printf(TEXT("Tinplate %d"), PlayerIndex);
	}
	return Result;
}

void UU0BotCreationComponent::SpawnOneBot()
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.OverrideLevel = GetComponentLevel();
	SpawnInfo.ObjectFlags |= RF_Transient;
	FVector Location = GetOwner()->GetActorLocation();
	FRotator Rotator = GetOwner()->GetActorRotation();
	AAIController* NewController = GetWorld()->SpawnActor<AAIController>(BotControllerClass, Location, Rotator, SpawnInfo);

	if (NewController != nullptr)
	{
		if (NewController->PlayerState != nullptr)
		{
			NewController->PlayerState->SetPlayerName(CreateBotName(NewController->PlayerState->GetPlayerId()));
		}

		FActorSpawnParameters SpawnPawnInfo;
		//SpawnPawnInfo.Instigator 
		SpawnPawnInfo.ObjectFlags |= RF_Transient;
		SpawnPawnInfo.bDeferConstruction = true;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location + FVector(200, 200, 100));

		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(BotPawnData->PawnClass, SpawnTransform, SpawnPawnInfo))
		{
			SpawnedPawn->FinishSpawning(SpawnTransform);
			if (AU0Monster* MonsterPawn = CastChecked<AU0Monster>(SpawnedPawn))
			{
				MonsterPawn->SetPawnData(BotPawnData);
				NewController->Possess(MonsterPawn);
			}

		}
		NewController->OnDestroyed.AddDynamic(this, &ThisClass::OnAIControllerDestroyed);
		SpawnedBotList.Add(NewController);
		bAllDie = false;
	}
}

void UU0BotCreationComponent::RemoveOneBot()
{
	if (SpawnedBotList.Num() > 0)
	{
		// Right now this removes a random bot as they're all the same; could prefer to remove one
		// that's high skill or low skill or etc... depending on why you are removing one
		const int32 BotToRemoveIndex = FMath::RandRange(0, SpawnedBotList.Num() - 1);

		AAIController* BotToRemove = SpawnedBotList[BotToRemoveIndex];
		SpawnedBotList.RemoveAtSwap(BotToRemoveIndex);

		if (BotToRemove)
		{
			// If we can find a health component, self-destruct it, otherwise just destroy the actor
			if (APawn* ControlledPawn = BotToRemove->GetPawn())
			{
				 
				if (UU0HealthComponent* HealthComponent = UU0HealthComponent::FindHealthComponent(ControlledPawn))
				{
					// Note, right now this doesn't work quite as desired: as soon as the player state goes away when
					// the controller is destroyed, the abilities like the death animation will be interrupted immediately
					// TODO : U0 HealthComponent - DamageSelfDestruct
					// HealthComponent->DamageSelfDestruct();
				}
				else
				{
					ControlledPawn->Destroy();
				}
			}

			// Destroy the controller (will cause it to Logout, etc...)
			BotToRemove->Destroy();
		}
	}
}

#else // !WITH_SERVER_CODE

#endif
