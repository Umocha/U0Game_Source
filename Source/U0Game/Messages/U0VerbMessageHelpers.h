// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "U0VerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FU0VerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;


UCLASS()
class U0GAME_API UU0VerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "U0")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "U0")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "U0")
	static FGameplayCueParameters VerbMessageToCueParameters(const FU0VerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "U0")
	static FU0VerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
