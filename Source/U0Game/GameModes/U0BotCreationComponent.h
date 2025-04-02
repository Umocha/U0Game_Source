// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "U0BotCreationComponent.generated.h"

class UU0ExperienceDefinition;
class AAIController;
class UU0PawnData;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FU0AllDie_Event);

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class U0GAME_API UU0BotCreationComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UU0BotCreationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	//~End of UActorComponent interface


protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	int32 NumBotsToCreate = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	TSubclassOf<AAIController> BotControllerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	TObjectPtr<UU0PawnData> BotPawnData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gameplay)
	TArray<FString> RandomBotNames;

	TArray<FString> RemainingBotNames;
protected:
	UPROPERTY(Transient)
	TArray<TObjectPtr<AAIController>> SpawnedBotList;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool bAllDie = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FU0AllDie_Event OnAllDead;

	/** Always creates a single bot */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Gameplay)
	virtual void SpawnOneBot();

	/** Deletes the last created bot if possible */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Gameplay)
	virtual void RemoveOneBot();

	/** Spawns bots up to NumBotsToCreate */
	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = Gameplay)
	void ServerCreateBots();

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	bool IsAllDie() const { return bAllDie; }

	UFUNCTION()
	void OnAIControllerDestroyed(AActor* DestroyedActor);

#if WITH_SERVER_CODE
public:
	FString CreateBotName(int32 PlayerIndex);
#endif
};
