// Copyright Epic Games, Inc. All Rights Reserved.

#include "U0Game.h"
#include "U0LogChannels.h"
#include "Modules/ModuleManager.h"
#include "System/U0AssetManager.h"

/*
* FU0GameModule
*/
class FU0GameModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FU0GameModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

	UU0AssetManager::TestClone();
}

void FU0GameModule::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}


IMPLEMENT_PRIMARY_GAME_MODULE(FU0GameModule, U0Game, "U0Game" );