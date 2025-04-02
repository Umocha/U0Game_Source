// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class U0Game : ModuleRules
{
	public U0Game(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{
			"Core",
			"CoreUObject",
			"Engine",
            "PhysicsCore",
			// Input
			"InputCore",
			"EnhancedInput",
			// GAS
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			// AI
			"AIModule",
			// Game Features
			"ModularGameplay",
			"GameFeatures",
			"ModularGameplayActors",
			// CommonUser
			"CommonUser",
			// CommonGame
			"CommonGame",
			// CommonUI
			"CommonUI",
			// CommonInput
            "CommonInput",
			// UMG
			"UMG",
			// UIExtension
			"UIExtension",
			// Slate
			"Slate",
			"SlateCore",
			// Niagara
            "Niagara",
			// LoadingScreen
            "CommonLoadingScreen",
            "AsyncMixin",
            "ControlFlows",
            "PropertyPath"
        });

		PrivateDependencyModuleNames.AddRange(
			new string[] {
                "DeveloperSettings",
                "EngineSettings",
                "GameplayMessageRuntime",
            });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
