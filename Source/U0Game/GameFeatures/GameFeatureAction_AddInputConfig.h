// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "U0Game/Input/U0MappableConfigPair.h"

#include "GameFeatureAction_AddInputConfig.generated.h"


class APawn;
struct FComponentRequestHandle;

/*
*
*/
UCLASS()
class UGameFeatureAction_AddInputConfig : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	/*
	* UGameFeatureAction's interface
	*/ 
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	/*
	* UGameFeatureAction_WorldActionBase's interface
	*/ 
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	
private:
	/** A way for us to keep references to any delegate handles that are needed and track the pawns that have been modified */
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
	};

	/* ActiveData를 초기화 */
	void Reset(FPerContextData& ActiveData);

	/* GameFeatureState가 변경될 때, 콜백 함수 (혹은 ExtensionEvent를 GameFrameworkComponentManager를 통해 호출 가능) */
	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	/* InputConfig를 EnhanceInputLocalPlayerSubsystem에 추가 및 제거 */
	void AddInputConfig(APawn* Pawn, FPerContextData& ActiveData);
	void RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData);

	/* world -> (extension request, pawn) */
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

};
