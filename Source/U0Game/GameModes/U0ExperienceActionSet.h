// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "U0ExperienceActionSet.generated.h"

class UGameFeatureAction;

/**
 * 
 */
UCLASS()
class U0GAME_API UU0ExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UU0ExperienceActionSet();

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif // WITH_EDITORONLY_DATA

	/*
	* member variables
	*/
	UPROPERTY(EditAnywhere, Category = "Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};
