// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "U0PawnExtensionComponent.generated.h"

class UU0PawnData;
class UU0AbilitySystemComponent;

/**
 * 초기화 전반을 조정하는 컴포넌트
 */
UCLASS()
class U0GAME_API UU0PawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UU0PawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/* FeatureName 정의	*/
	static const FName NAME_ActorFeatureName;

	/*
	* member methods
	*/
	static UU0PawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UU0PawnExtensionComponent>() : nullptr); }
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void SetPawnData(const UU0PawnData* InPawnData);
	void SetupPlayerInputComponent();
	UU0AbilitySystemComponent* GetU0AbilitySystemComponent() const { return AbilitySystemComponent; }
	
	/* AbilitySystemComponent의 AvatarActor 대상 초기화/해제 호출 */
	void InitializeAbilitySystem(UU0AbilitySystemComponent* InASC, AActor* InOwnerActor);
	void UninitializeAbilitySystem();

	/* OnAbilitySystem[Initialized|Uninitialzed] Delegate에 추가 */
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

	/*
	* UPawnComponent interfaces
	*/
	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	/*
	* IGameFrameworkInitStateInterface
	*/
	virtual FName GetFeatureName() const final { return NAME_ActorFeatureName; }
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void CheckDefaultInitialization() final;

	/* ASC Init과 Uninit의 Delegate 추가 */
	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	/* Pawn을 생성한 데이터를 캐싱 */
	UPROPERTY(EditInstanceOnly, Category = "U0|Pawn")
	TObjectPtr<const UU0PawnData> PawnData;

	/* AbilitySystemComponent 캐싱 */
	UPROPERTY()
	TObjectPtr<UU0AbilitySystemComponent> AbilitySystemComponent;

};
