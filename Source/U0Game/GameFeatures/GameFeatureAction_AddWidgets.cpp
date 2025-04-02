// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatureAction_AddWidgets.h"
#include "Components/GameFrameworkComponentManager.h"
#include "U0Game/UI/U0HUD.h"
#include "CommonUIExtensions.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddWidgets)

void UGameFeatureAction_AddWidgets::AddWidgets(AActor* Actor, FPerContextData& ActiveData)
{
	AU0HUD* HUD = CastChecked<AU0HUD>(Actor);

	// HUD를 통해, LocalPlayer를 가져오자
	if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(HUD->GetOwningPlayerController()->Player))
	{
		FPerActorData& ActorData = ActiveData.ActorData.FindOrAdd(HUD);

		// Layout의 요청을 순회하자 (보통 하나만 들어가 있긴하다)
		for (const FU0HUDLayoutRequest& Entry : Layout)
		{
			if (TSubclassOf<UCommonActivatableWidget> ConcreteWidgetClass = Entry.LayoutClass.Get())
			{
				ActorData.LayoutsAdded.Add(UCommonUIExtensions::PushContentToLayer_ForPlayer(LocalPlayer, Entry.LayerID, ConcreteWidgetClass));
			}
		}

		// Widget을 순회하며, UIExtensionSubsystem의 Extension에 추가한다
		UUIExtensionSubsystem* ExtensionSubsystem = HUD->GetWorld()->GetSubsystem<UUIExtensionSubsystem>();
		for (const FU0HUDElementEntry& Entry : Widgets)
		{
			ActorData.ExtensionHandles.Add(ExtensionSubsystem->RegisterExtensionAsWidgetForContext(Entry.SlotID, LocalPlayer, Entry.WidgetClass.Get(), -1));
		}
	}
}

void UGameFeatureAction_AddWidgets::RemoveWidgets(AActor* Actor, FPerContextData& ActiveData)
{
	AU0HUD* HUD = CastChecked<AU0HUD>(Actor);

	FPerActorData* ActorData = ActiveData.ActorData.Find(HUD);

	if (ActorData)
	{
		// U0HUD에 추가된 CommonActivatableWidget을 순회하며, Deativate 시켜준다
		for (TWeakObjectPtr<UCommonActivatableWidget>& AddedLayout : ActorData->LayoutsAdded)
		{
			if (AddedLayout.IsValid())
			{
				AddedLayout->DeactivateWidget();
			}
		}

		// UIExtension에 대해 순회하며, Unregister() 한다
		for (FUIExtensionHandle& Handle : ActorData->ExtensionHandles)
		{
			// Unregiser()는 UIIExtensionSystem에서 제거가 올바르게 되야 한다
			Handle.Unregister();
		}

		ActiveData.ActorData.Remove(HUD);
	}
}

void UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	FPerContextData* ActiveData = ContextData.Find(Context);
	if ensure(ActiveData)
	{
		Reset(*ActiveData);
	}

}

void UGameFeatureAction_AddWidgets::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// GameFrameworkComponentManager를 가져올 GameInstance와 World는 GameWorld여야 한다
	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		// GameFrameworkComponentManager를 가져옴
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			// 기본적으로 Widget을 추가할 대상으로 AU0HUD를 고정한다
			TSoftClassPtr<AActor> HUDActorClass = AU0HUD::StaticClass();
		
			// GFA_WorldBase와 마찬가지로 HandleActorExtension을 callback으로 받도록 하자
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(
				HUDActorClass,
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleActorExtension, ChangeContext));
		
			ActiveData.ComponentRequests.Add(ExtensionRequestHandle);
		}
	}
}

void UGameFeatureAction_AddWidgets::Reset(FPerContextData& ActiveData)
{
	ActiveData.ComponentRequests.Empty();

	for (TPair<FObjectKey, FPerActorData>& Pair : ActiveData.ActorData)
	{
		for (FUIExtensionHandle& Handle : Pair.Value.ExtensionHandles)
		{
			Handle.Unregister();
		}
	}
	ActiveData.ActorData.Empty();
}

void UGameFeatureAction_AddWidgets::HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// Receiver(AU0HUD)의 Removed/Added에 따라 Widget을 추가/제거 해준다
	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveWidgets(Actor, ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UGameFrameworkComponentManager::NAME_GameActorReady))
	{
		AddWidgets(Actor, ActiveData);
	}

}
