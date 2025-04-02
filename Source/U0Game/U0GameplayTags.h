// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

namespace U0GameplayTags
{
	U0GAME_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// Declare all of the custom native tags that Lyra will use
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action);

	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_PartySlot_SelectSlot);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_PartySlot_Select_Slot1);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_PartySlot_Select_Slot2);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Ability_PartySlot_Select_Slot3);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_ZoomInOut_Mouse);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Cursor_Mouse_Press);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Cursor_Mouse_Release);

	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_InCombat);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Knockback);

	// These are mappings from MovementMode enums to GameplayTags associated with those enums (below)
	U0GAME_API	extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	U0GAME_API	extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);

	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);


	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Dash_Used);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Dash_Just);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Intro);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Intro_Possibility);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Outro);
	U0GAME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestRespawn);

	U0GAME_API	extern const TArray<FGameplayTag> OnLandedRemoveTag;
};
