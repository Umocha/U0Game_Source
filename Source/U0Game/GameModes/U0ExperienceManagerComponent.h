// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameFeaturePluginOperationResult.h"
#include "LoadingProcessInterface.h"

#include "U0ExperienceManagerComponent.generated.h"

class UU0ExperienceDefinition;

enum class EU0ExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	ExcutingActions,
	Loaded,
	Deactivating,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnU0ExperienceLoaded, const UU0ExperienceDefinition*);

/**
 * U0ExperienceManagerComponent
 * - 말 그대로 해당 component는 game state를 owner로 가지면서, experience의 상태 정보를 가지고 있는 component이다
 * - 뿐만 아니라, manager이라는 단어가 포함되어 있듯, experience 로딩 상태 업데이트 및 이벤트를 관리한다
 */
UCLASS()
class U0GAME_API UU0ExperienceManagerComponent : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()
public:
	UU0ExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	//~ILoadingProcessInterface interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~End of ILoadingProcessInterface

	/*
	* member methods
	*/
	bool IsExperienceLoaded() { return (LoadState == EU0ExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

	/*
	* 아래의 OnExperienceLoaded에 바인딩하거나, 이미 Experience 로딩이 완료되었다면 바로 호출함
	*/
	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnU0ExperienceLoaded::FDelegate&& Delegate);
	void CallOrRegister_OnExperienceLoaded(FOnU0ExperienceLoaded::FDelegate&& Delegate);
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnU0ExperienceLoaded::FDelegate&& Delegate);

	void ServerSetCurrentExperience(FPrimaryAssetId ExperienceId);
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();
	const UU0ExperienceDefinition* GetCurrentExperienceChecked() const;

	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();

public:
	UPROPERTY()
	TObjectPtr<const UU0ExperienceDefinition> CurrentExperience;

	/* Experience의 로딩 상태를 모니터링 */
	EU0ExperienceLoadState LoadState = EU0ExperienceLoadState::Unloaded;

	/* Experience 로딩이 완료된 이후, Broadcasting Delegate */
	FOnU0ExperienceLoaded OnExperienceLoaded_HighPriority;
	FOnU0ExperienceLoaded OnExperienceLoaded;
	FOnU0ExperienceLoaded OnExperienceLoaded_LowPriority;

	/* 활성화된 GameFeature Plugin들 */
	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;

	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;
};
