// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "U0ExperienceDefinition.generated.h"

class UU0PawnData;
class UU0ExperienceActionSet;
class UGameFeatureAction;

/**
 * 
 */
UCLASS()
class U0GAME_API UU0ExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UU0ExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif // WITH_EDITORONLY_DATA


	/*
	* member variables
	*/
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TObjectPtr<UU0PawnData> DefaultPawnData;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	/* ExperienceActionSet은 UGameFeatureAction의 Set이며, Gameplay 용도에 맞게 분류의 목적으로 사용한다 */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<TObjectPtr<UU0ExperienceActionSet>> ActionSets;

	/* 일반적인 GameFeatureAction으로서 추가 */
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

};
