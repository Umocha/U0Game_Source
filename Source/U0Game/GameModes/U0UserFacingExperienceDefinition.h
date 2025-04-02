// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "U0UserFacingExperienceDefinition.generated.h"

class UCommonSession_HostSessionRequest;

/**
 * 
 */
UCLASS()
class U0GAME_API UU0UserFacingExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/*
	* Map 로딩 및 Experience 전환을 위해, MapID와 ExperienceID를 활용하여, HostSessionRequest 생성
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UCommonSession_HostSessionRequest* CreateHostingRequest() const;

	/** the specific map to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	/** the gameplay experience to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "U0ExperienceDefinition"))
	FPrimaryAssetId ExperienceID;

	/** Primary title in the UI */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	FText TileTitle;
};
