#include "U0EditorModule.h"

#include "ToolMenu.h"
#include "Framework/Application/SlateApplication.h"
#include "Private/AssetTypeActions_U0ContextEffectsLibrary.h"

#define LOCTEXT_NAMESPACE "U0Editor"

DEFINE_LOG_CATEGORY(LogU0Editor);

/**
 * FU0EditorModule
 */
class FU0EditorModule : public FDefaultGameModuleImpl
{
	typedef FU0EditorModule ThisClass;

	virtual void StartupModule() override
	{
		// FGameEditorStyle::Initialize();

		if (!IsRunningGame())
		{
			//FModuleManager::Get().OnModulesChanged().AddRaw(this, &FU0EditorModule::ModulesChangedCallback);

			//BindGameplayAbilitiesEditorDelegates();

			if (FSlateApplication::IsInitialized())
			{
				// ToolMenusHandle = UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&RegisterGameEditorMenus));
			}

			FEditorDelegates::BeginPIE.AddRaw(this, &ThisClass::OnBeginPIE);
			FEditorDelegates::EndPIE.AddRaw(this, &ThisClass::OnEndPIE);
		}

		// Register the Context Effects Library asset type actions.
		{
			IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
			TSharedRef<FAssetTypeActions_U0ContextEffectsLibrary> AssetAction = MakeShared<FAssetTypeActions_U0ContextEffectsLibrary>();
			U0ContextEffectsLibraryAssetAction = AssetAction;
			AssetTools.RegisterAssetTypeActions(AssetAction);
		}
	}

	void OnBeginPIE(bool bIsSimulating)
	{
		//UU0ExperienceManager* ExperienceManager = GEngine->GetEngineSubsystem<UU0ExperienceManager>();
		//check(ExperienceManager);
		//ExperienceManager->OnPlayInEditorBegun();
	}

	void OnEndPIE(bool bIsSimulating)
	{
	}

	virtual void ShutdownModule() override
	{
		// Unregister the Context Effects Library asset type actions.
		{
			FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools");
			TSharedPtr<IAssetTypeActions> AssetAction = U0ContextEffectsLibraryAssetAction.Pin();
			if (AssetToolsModule && AssetAction)
			{
					AssetToolsModule->Get().UnregisterAssetTypeActions(AssetAction.ToSharedRef());
			}
		}

		FEditorDelegates::BeginPIE.RemoveAll(this);
		FEditorDelegates::EndPIE.RemoveAll(this);

		// Undo UToolMenus
		if (UObjectInitialized() && ToolMenusHandle.IsValid())
		{
			UToolMenus::UnRegisterStartupCallback(ToolMenusHandle);
		}

		//UnbindGameplayAbilitiesEditorDelegates();
		FModuleManager::Get().OnModulesChanged().RemoveAll(this);
		//FGameEditorStyle::Shutdown();
	}

protected:

	/*static void BindGameplayAbilitiesEditorDelegates()
	{
		IGameplayAbilitiesEditorModule& GameplayAbilitiesEditorModule = IGameplayAbilitiesEditorModule::Get();

		GameplayAbilitiesEditorModule.GetGameplayCueNotifyClassesDelegate().BindStatic(&GetGameplayCueDefaultClasses);
		GameplayAbilitiesEditorModule.GetGameplayCueInterfaceClassesDelegate().BindStatic(&GetGameplayCueInterfaceClasses);
		GameplayAbilitiesEditorModule.GetGameplayCueNotifyPathDelegate().BindStatic(&GetGameplayCuePath);
	}

	static void UnbindGameplayAbilitiesEditorDelegates()
	{
		if (IGameplayAbilitiesEditorModule::IsAvailable())
		{
			IGameplayAbilitiesEditorModule& GameplayAbilitiesEditorModule = IGameplayAbilitiesEditorModule::Get();
			GameplayAbilitiesEditorModule.GetGameplayCueNotifyClassesDelegate().Unbind();
			GameplayAbilitiesEditorModule.GetGameplayCueInterfaceClassesDelegate().Unbind();
			GameplayAbilitiesEditorModule.GetGameplayCueNotifyPathDelegate().Unbind();
		}
	}

	void ModulesChangedCallback(FName ModuleThatChanged, EModuleChangeReason ReasonForChange)
	{
		if ((ReasonForChange == EModuleChangeReason::ModuleLoaded) && (ModuleThatChanged.ToString() == TEXT("GameplayAbilitiesEditor")))
		{
			BindGameplayAbilitiesEditorDelegates();
		}
	}*/

private:
	TWeakPtr<IAssetTypeActions> U0ContextEffectsLibraryAssetAction;
	FDelegateHandle ToolMenusHandle;
};

IMPLEMENT_MODULE(FU0EditorModule, U0Editor);

#undef LOCTEXT_NAMESPACE