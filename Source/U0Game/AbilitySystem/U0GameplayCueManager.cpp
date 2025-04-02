// Fill out your copyright notice in the Description page of Project Settings.


#include "U0GameplayCueManager.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueSet.h"
#include "Engine/AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0GameplayCueManager)

UU0GameplayCueManager::UU0GameplayCueManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UU0GameplayCueManager* UU0GameplayCueManager::Get()
{
	return Cast<UU0GameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

const FPrimaryAssetType UFortAssetManager_GameplayCueRefsType = TEXT("GameplayCueRefs");
const FName UFortAssetManager_GameplayCueRefsName = TEXT("GameplayCueReferences");
const FName UFortAssetManager_LoadStateClient = FName(TEXT("Client"));

void UU0GameplayCueManager::RefreshGameplayCuePrimaryAsset()
{
	TArray<FSoftObjectPath> CuePaths;
	UGameplayCueSet* RuntimeGameplayCueSet = GetRuntimeCueSet();
	if (RuntimeGameplayCueSet)
	{
		RuntimeGameplayCueSet->GetSoftObjectPaths(CuePaths);
	}

	// 새로 추가된 에셋의 경로를 CuePaths에 추가하여!
	FAssetBundleData BundleData;
	BundleData.AddBundleAssetsTruncated(UFortAssetManager_LoadStateClient, CuePaths);

	// PrimaryAssetId를 고정된 이름으로 설정하여 (const로 설정되어 있음)
	FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(UFortAssetManager_GameplayCueRefsType, UFortAssetManager_GameplayCueRefsName);

	// 에셋 매니저에 추가 진행
	UAssetManager::Get().AddDynamicAsset(PrimaryAssetId, FSoftObjectPath(), BundleData);
}
