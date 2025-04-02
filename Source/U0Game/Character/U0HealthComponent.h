// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "U0HealthComponent.generated.h"

/* forward declarations */
class UU0HealthComponent;

class UU0AbilitySystemComponent;
class UU0HealthSetBase;
struct FGameplayEffectSpec;
struct FPawnAttributeData;

/* Health 변화 callback을 위한 Delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FU0Health_AttributeChanged, UU0HealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FU0Health_DeathEvent, AActor*, OwningActor);

UENUM(BlueprintType)
enum class EU0DeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};


/**
 * Character(Pawn)에 대해 체력관련 처리를 담당하는 Component이다
 * - 참고로 해당 클래스는 Blueprintable이다:
 * - 이는 멤버변수인 Delegate를 UI에서 바인딩하기 위함이다
 */
UCLASS()
class U0GAME_API UU0HealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()
public:
	UU0HealthComponent(const FObjectInitializer& ObjectInitializer);

	/* Actor(보통 ACharacter/APawn)의 HealthComponent를 반환 */
	UFUNCTION(BlueprintPure, Category = "U0|Health")
	static UU0HealthComponent* FindHealthComponent(const AActor* Actor);

	/* 아래의 UFUNCTION은 HealthSet의 Attribute에 접근하기 위한 BP Accessor 함수들 */
	UFUNCTION(BlueprintCallable, Category = "U0|Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Health")
	float GetHealthNormalized() const;

	/* ASC와 HealthSet 초기화 */
	void InitializeWithAbilitySystem(UU0AbilitySystemComponent* InASC, const FPawnAttributeData& AttributeData, int32 SlotIndex = INDEX_NONE);
	void UninitializeWithAbilitySystem();

	UFUNCTION(BlueprintCallable, Category = "U0|Health")
	EU0DeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "U0|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EU0DeathState::NotDead); }

	// Begins the death sequence for the owner.
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

	virtual void Revive();

	/* ASC를 통해 HealthSet의 HealthAttribute 변경이 있을 때 호출하는 메서드 (내부적으로 OnHealthChanged 호출) */
	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);


	/* HealthSet에 접근하기 위한 AbilitySystemComponent */
	UPROPERTY()
	TObjectPtr<UU0AbilitySystemComponent> AbilitySystemComponent;

	/* 캐싱된 HealthSet 레퍼런스 */
	UPROPERTY()
	TObjectPtr<const UU0HealthSetBase> HealthSet;

	UPROPERTY()
	EU0DeathState DeathState;

	/* Health 변화에 따른 Delegate(Multicast) */
	UPROPERTY(BlueprintAssignable)
	FU0Health_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FU0Health_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FU0Health_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FU0Health_DeathEvent OnDeathFinished;

	UPROPERTY(BlueprintAssignable)
	FU0Health_DeathEvent OnRevived;
};
