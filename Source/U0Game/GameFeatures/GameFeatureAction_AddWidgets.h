// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "UIExtensionSystem.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "GameFeatureAction_AddWidgets.generated.h"

/* forward declarations */
struct FComponentRequestHandle;

/* HUD의 Layout 요청 */
USTRUCT()
struct FU0HUDLayoutRequest
{
	GENERATED_BODY()
public:
	/* UI의 Layout으로 CommonActivatableWidget을 사용 */
	UPROPERTY(EditAnywhere, Category = "UI", meta = (AssetBundles = "Client"))
	TSoftClassPtr<UCommonActivatableWidget> LayoutClass;

	/* 앞서 보았던 PrimaryGameLayout의 LayerID를 의미 */
	UPROPERTY(EditAnywhere, Category = "UI")
	FGameplayTag LayerID;
};

USTRUCT()
struct FU0HUDElementEntry
{
	GENERATED_BODY()
public:
	/* U0HUDLayout 위에 올릴 대상이 되는 Widget Class */
	UPROPERTY(EditAnywhere, Category = "UI", meta = (AssetBundles = "Client"))
	TSoftClassPtr<UUserWidget> WidgetClass;

	/* SlotID는 U0HUDLayoutRequest에 올린 LayoutClass에 정의된 Slot(GameplayTag)를 의미 */
	UPROPERTY(EditAnywhere, Category = "UI")
	FGameplayTag SlotID;
};


/**
 * 
 */
UCLASS()
class U0GAME_API UGameFeatureAction_AddWidgets : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()
public:
	struct FPerActorData
	{
		TArray<TWeakObjectPtr<UCommonActivatableWidget>> LayoutsAdded;
		TArray<FUIExtensionHandle> ExtensionHandles;
	};

	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
		TMap<FObjectKey, FPerActorData> ActorData;
	};

	void AddWidgets(AActor* Actor, FPerContextData& ActiveData);
	void RemoveWidgets(AActor* Actor, FPerContextData& ActiveData);

	/*
	* UGameFeatureAction's interface
	*/
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	/*
	* UGameFeatureAction_WorldActionBase's interface
	*/
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

	void Reset(FPerContextData& ActiveData);

	void HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	/* GFA Add/Remove 상태 관리 */
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	/* GFA_AddWidget은 형태를 정의하는 Layout과 Layout 위에 올릴 Widget객체 Widgets으로 구성된다 */
	UPROPERTY(EditAnywhere, Category = "UI")
	TArray<FU0HUDLayoutRequest> Layout;

	UPROPERTY(EditAnywhere, Category = "UI")
	TArray<FU0HUDElementEntry> Widgets;

};
