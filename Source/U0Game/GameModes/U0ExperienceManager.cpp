// Copyright Epic Games, Inc. All Rights Reserved.

#include "U0ExperienceManager.h"
#include "Engine/Engine.h"
#include "Subsystems/SubsystemCollection.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0ExperienceManager)

#if WITH_EDITOR

void UU0ExperienceManager::OnPlayInEditorBegun()
{
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}

void UU0ExperienceManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		UU0ExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UU0ExperienceManager>();
		check(ExperienceManagerSubsystem);

		// Track the number of requesters who activate this plugin. Multiple load/activation requests are always allowed because concurrent requests are handled.
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
		++Count;
	}
}

bool UU0ExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	/*
	if (GIsEditor)
	{
		UU0ExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<UU0ExperienceManager>();
		check(ExperienceManagerSubsystem);

		// Only let the last requester to get this far deactivate the plugin
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
		--Count;

		if (Count == 0)
		{
			ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);
			return true;
		}

		return false;
	}

	return true;
	*/

	return true;
}

#endif
