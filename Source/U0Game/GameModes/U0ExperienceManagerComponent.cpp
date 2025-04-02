// Fill out your copyright notice in the Description page of Project Settings.


#include "U0ExperienceManagerComponent.h"
#include "U0ExperienceDefinition.h"
#include "U0ExperienceActionSet.h"
#include "U0Game/System/U0AssetManager.h"
#include "GameFeaturesSubsystemSettings.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction.h"
#include "U0Game/U0LogChannels.h"
#include "U0ExperienceManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0ExperienceManagerComponent)

UU0ExperienceManagerComponent::UU0ExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UU0ExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// deactivate any features this experience loaded
//@TODO: This should be handled FILO as well
	for (const FString& PluginURL : GameFeaturePluginURLs)
	{
		if (UU0ExperienceManager::RequestToDeactivatePlugin(PluginURL))
		{
			UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
		}
	}

	//@TODO: Ensure proper handling of a partially-loaded state too
	if (LoadState == EU0ExperienceLoadState::Loaded)
	{
		LoadState = EU0ExperienceLoadState::Deactivating;

		// Make sure we won't complete the transition prematurely if someone registers as a pauser but fires immediately
		NumExpectedPausers = INDEX_NONE;
		NumObservedPausers = 0;

		// Deactivate and unload the actions
		FGameFeatureDeactivatingContext Context(TEXT(""), [this](FStringView) { this->OnActionDeactivationCompleted(); });

		const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
		if (ExistingWorldContext)
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		auto DeactivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				for (UGameFeatureAction* Action : ActionList)
				{
					if (Action)
					{
						Action->OnGameFeatureDeactivating(Context);
						Action->OnGameFeatureUnregistering();
					}
				}
			};

		DeactivateListOfActions(CurrentExperience->Actions);
		for (const TObjectPtr<UU0ExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
		{
			if (ActionSet != nullptr)
			{
				DeactivateListOfActions(ActionSet->Actions);
			}
		}

		NumExpectedPausers = Context.GetNumPausers();

		if (NumExpectedPausers > 0)
		{
			UE_LOG(LogU0, Error, TEXT("Actions that have asynchronous deactivation aren't fully supported yet in U0 experiences"));
		}

		if (NumExpectedPausers == NumObservedPausers)
		{
			OnAllActionsDeactivated();
		}
	}
}

bool UU0ExperienceManagerComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (LoadState != EU0ExperienceLoadState::Loaded)
	{
		OutReason = TEXT("Experience still loading");
		return true;
	}
	else
	{
		return false;
	}
}

void UU0ExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_HighPriority(FOnU0ExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded_HighPriority.Add(MoveTemp(Delegate));
	}
}

void UU0ExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnU0ExperienceLoaded::FDelegate&& Delegate)
{
	// IsExperienceLoaded() 구현
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		// delegate_type 내부에 변수때문에 복사 비용을 낮추기 위해 Move를 이용하도록 하자
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UU0ExperienceManagerComponent::CallOrRegister_OnExperienceLoaded_LowPriority(FOnU0ExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		OnExperienceLoaded_LowPriority.Add(MoveTemp(Delegate));
	}
}

void UU0ExperienceManagerComponent::ServerSetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	UU0AssetManager& AssetManager = UU0AssetManager::Get();

	TSubclassOf<UU0ExperienceDefinition> AssetClass;
	{
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
		AssetClass = Cast<UClass>(AssetPath.TryLoad());
	}

	// 왜 CDO를 가져오는 걸까?
	const UU0ExperienceDefinition* Experience = GetDefault<UU0ExperienceDefinition>(AssetClass);
	check(Experience != nullptr);
	check(CurrentExperience == nullptr);
	{
		// 그리고 CDO로 CurrentExperience를 설정한다.
		// 어떤 의도로 이렇게 코드를 작성한지는 코드를 쭉 읽어보고(StartExperienceLoad까지 읽어보자) 다시 생각해보자:
		CurrentExperience = Experience;
	}

	StartExperienceLoad();
}

#pragma region MyRegion
//void UU0ExperienceManagerComponent::StartExperienceLoad()
//{
//	check(CurrentExperience);
//	check(LoadState == EU0ExperienceLoadState::Unloaded);
//
//	LoadState = EU0ExperienceLoadState::Loading;
//
//	UU0AssetManager& AssetManager = UU0AssetManager::Get();
//
//	// 이미 앞서, ServerSetCurrentExperience에서 우리는 ExperienceId를 넘겨주었는데, 여기서 CDO를 활용하여, GetPrimaryAssetId를 로딩할 대상으로 넣는다!
//	// - 왜 이렇게 하는걸까?
//	// - GetPrimaryAssetId가 작동하는 것을 살펴보자:
//	// - 두가지를 알아 낼수 있다.
//	//		1. 우리는 B_U0DefaultExperience를 BP로 만든 이유
//	//		2. CDO를 가져와서, GetPrimaryAssetId를 호출한 이유
//
//	// 우리는 앞서 이미 CDO로 로딩했다
//	// CDO를 그대로 사용하지 않고 CDO를 통해 로딩할 에셋을 지정하여, BundleAssetList에 추가해준다.
//	TSet<FPrimaryAssetId> BundleAssetList;
//	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());
//
//	// ExperienceActionSet의 순회하며, BundleAssetList로 추가하자:
//	for (const TObjectPtr<UU0ExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
//	{
//		if (ActionSet)
//		{
//			// 앞서, 우리가 생성한 UAS_Shooter_SharedHUD가 추가되겠다 (물론 추가적인 UAS_Shooter_XXX)도 추가될거다
//			// - BundleAssetList는 Bundle로 등록할 Root의 PrimaryDataAsset를 추가하는 과정이다
//			//   (->??? 무슨말인가 싶은데 ChangeBundleStateForPrimaryAssets)을 살펴보면서 이해하자
//			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
//		}
//	}
//
//	// load assets associated with the experience
//	// 아래는 우리가 후일 GameFeature를 사용하여, Experience에 바인딩된 GameFeature Plugin을 로딩할 Bundle 이름을 추가한다:
//	// - Bundle이라는게 후일 우리가 로딩할 에셋의 카테고리 이름이라고 생각하면 된다 (일단 지금은 넘어가자 후일, 또 다룰것임)
//	TArray<FName> BundlesToLoad;
//	{
//		// 여기서 주목해야 할 부분은 OwnerNetMode가 NM_Standalone이면? Client/Server 둘다 로딩에 추가된다!
//		const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
//		bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
//		bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
//		if (bLoadClient)
//		{
//			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
//		}
//		if (bLoadServer)
//		{
//			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
//		}
//	}
//
//	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplate);
//
//	// 아래도, 후일 Bundle을 우리가 GameFeature에 연동하면서 더 깊게 알아보기로 하고, 지금은 앞서 B_U0DefaultExperience를 로딩해주는 함수로 생각하자
//	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
//		BundleAssetList.Array(),
//		BundlesToLoad,
//		{}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);
//
//	if (!Handle.IsValid() || Handle->HasLoadCompleted())
//	{
//		// 로딩이 완료되었으면, ExecuteDelegate를 통해 OnAssetsLoadedDelegate를 호출하자:
//		// - 아래의 함수를 확인해보자:
//		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
//	}
//	else
//	{
//		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
//		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
//			{
//				OnAssetsLoadedDelegate.ExecuteIfBound();
//			}));
//	}
//
//	// FrameNumber를 주목해서 보자
//	static int32 StartExperienceLoad_FrameNumber = GFrameNumber;
//}
#pragma endregion

void UU0ExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience != nullptr);
	check(LoadState == EU0ExperienceLoadState::Unloaded);

	LoadState = EU0ExperienceLoadState::Loading;

	UU0AssetManager& AssetManager = UU0AssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	TSet<FSoftObjectPath> RawAssetList;

	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());
	for (const TObjectPtr<UU0ExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	// Load assets associated with the experience

	TArray<FName> BundlesToLoad;

	//@TODO: Centralize this client/server stuff into the U0AssetManager
	const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
	const bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
	const bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
	if (bLoadClient)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}
	if (bLoadServer)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	}

	TSharedPtr<FStreamableHandle> BundleLoadHandle = nullptr;
	if (BundleAssetList.Num() > 0)
	{
		BundleLoadHandle = AssetManager.ChangeBundleStateForPrimaryAssets(BundleAssetList.Array(), BundlesToLoad, {}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);
	}

	TSharedPtr<FStreamableHandle> RawLoadHandle = nullptr;
	if (RawAssetList.Num() > 0)
	{
		RawLoadHandle = AssetManager.LoadAssetList(RawAssetList.Array(), FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority, TEXT("StartExperienceLoad()"));
	}

	// If both async loads are running, combine them
	TSharedPtr<FStreamableHandle> Handle = nullptr;
	if (BundleLoadHandle.IsValid() && RawLoadHandle.IsValid())
	{
		Handle = AssetManager.GetStreamableManager().CreateCombinedHandle({ BundleLoadHandle, RawLoadHandle });
	}
	else
	{
		Handle = BundleLoadHandle.IsValid() ? BundleLoadHandle : RawLoadHandle;
	}

	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);
	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		// Assets were already loaded, call the delegate now
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);

		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
			{
				OnAssetsLoadedDelegate.ExecuteIfBound();
			}));
	}

	// This set of assets gets preloaded, but we don't block the start of the experience based on it
	TSet<FPrimaryAssetId> PreloadAssetList;
	//@TODO: Determine assets to preload (but not blocking-ly)
	if (PreloadAssetList.Num() > 0)
	{
		AssetManager.ChangeBundleStateForPrimaryAssets(PreloadAssetList.Array(), BundlesToLoad, {});
	}
}


void UU0ExperienceManagerComponent::OnExperienceLoadComplete()
{
	// FrameNumber를 주목해 보자
	static int32 OnExperienceLoadComplate_FrameNumber = GFrameNumber;

	check(LoadState == EU0ExperienceLoadState::Loading);
	check(CurrentExperience);

	// 이전 활성화 GameFeature Plugin의 URL을 클리어 해준다
	GameFeaturePluginURLs.Reset();

	// 이걸 굳이 람다로 한 이유가 있나?
	auto CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
		{
			// FeaturePluginList를 순회하면서, PluginURL을 ExperienceManagerComponent의 GameFeaturePluginURLs에 추가해준다
			for (const FString& PluginName : FeaturePluginList)
			{
				FString PluginURL;
				if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
				{
					This->GameFeaturePluginURLs.AddUnique(PluginURL);
				}
			}
		};

	// GameFeaturesToEnable에 있는 Plugin만 일단 활성화할 GameFeature Plugin 후보군으로 등록
	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);

	// GameFeaturePluginURLs에 등록된 Plugin을 로딩 및 활성화:
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if (NumGameFeaturePluginsLoading)
	{
		LoadState = EU0ExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			// 매 Plugin이 로딩 및 활성화 이후, OnGameFeaturePluginLoadComplete 콜백 함수 등록
			// 해당 함수를 살펴보도록 하자
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		// 해당 함수가 불리는 것은 앞서 보았던 StreamableDelegateDelayHelper에 의해 불림
		OnExperienceFullLoadCompleted();
	}
}

void UU0ExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	// 매 GameFeature Plugin이 로딩될 때, 해당 함수가 콜백으로 불린다
	NumGameFeaturePluginsLoading--;
	if (NumGameFeaturePluginsLoading == 0)
	{
		// GameFeaturePlugin 로딩이 다 끝났을 경우, 기존대로 Loaded로서, OnExperienceFullLoadCompleted 호출한다
		// GameFeaturePlugin 로딩과 활성화가 끝났다면? UGameFeatureAction을 활성화 해야됨
		OnExperienceFullLoadCompleted();
	}
}

void UU0ExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != EU0ExperienceLoadState::Loaded);

	// 
	{
		LoadState = EU0ExperienceLoadState::ExcutingActions;

		// GameFeatureAction 활성화를 위한 Context 준비
		FGameFeatureActivatingContext Context;
		{
			// 월드의 핸들을 세팅해준다
			const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
			if (ExistingWorldContext)
			{
				Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
			}
		}

		auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				for (UGameFeatureAction* Action : ActionList)
				{
					// 명시적으로 GameFeatureAction에 대해 Registering -> Loading -> Activating 순으로 호출한다
					if (Action)
					{
						Action->OnGameFeatureRegistering();
						Action->OnGameFeatureLoading();
						Action->OnGameFeatureActivating(Context);
					}
				}
			};

		// 1. Experience의 Actions
		ActivateListOfActions(CurrentExperience->Actions);

		// 2. Experience의 ActionSets
		for (const TObjectPtr<UU0ExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	LoadState = EU0ExperienceLoadState::Loaded;

	OnExperienceLoaded_HighPriority.Broadcast(CurrentExperience);
	OnExperienceLoaded_HighPriority.Clear();

	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();

	OnExperienceLoaded_LowPriority.Broadcast(CurrentExperience);
	OnExperienceLoaded_LowPriority.Clear();
}

const UU0ExperienceDefinition* UU0ExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EU0ExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}

void UU0ExperienceManagerComponent::OnActionDeactivationCompleted()
{
	check(IsInGameThread());
	++NumObservedPausers;

	if (NumObservedPausers == NumExpectedPausers)
	{
		OnAllActionsDeactivated();
	}
}

void UU0ExperienceManagerComponent::OnAllActionsDeactivated()
{
	//@TODO: We actually only deactivated and didn't fully unload...
	LoadState = EU0ExperienceLoadState::Unloaded;
	CurrentExperience = nullptr;
	//@TODO:	GEngine->ForceGarbageCollection(true);
}
