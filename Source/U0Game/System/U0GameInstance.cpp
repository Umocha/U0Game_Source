// Fill out your copyright notice in the Description page of Project Settings.


#include "U0GameInstance.h"
#include "Components/GameFrameworkComponentManager.h"
#include "U0Game/U0GameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0GameInstance)


void UU0GameInstance::Init()
{
	Super::Init();

	// 앞서 정의한 InitState의 GameplayTags 등록
	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>();
	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(U0GameplayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(U0GameplayTags::InitState_DataAvailable, false, U0GameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(U0GameplayTags::InitState_DataInitialized, false, U0GameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(U0GameplayTags::InitState_GameplayReady, false, U0GameplayTags::InitState_DataInitialized);
	}
}

void UU0GameInstance::Shutdown()
{
	Super::Shutdown();
}
