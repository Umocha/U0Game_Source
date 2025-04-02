// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "U0SkillComponent.generated.h"

/* forward declarations */
class UU0HealthComponent;

class UU0AbilitySystemComponent;
class UU0HeroSkillSetBase;
struct FGameplayEffectSpec;
struct FCharacterAttributeData;

/* Health 변화 callback을 위한 Delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FU0Skill_AttributeChanged, UU0SkillComponent*, SkillComponent, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * Character(Pawn)에 대해 체력관련 처리를 담당하는 Component이다
 * - 참고로 해당 클래스는 Blueprintable이다:
 * - 이는 멤버변수인 Delegate를 UI에서 바인딩하기 위함이다
 */
UCLASS()
class U0GAME_API UU0SkillComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()
public:
	UU0SkillComponent(const FObjectInitializer& ObjectInitializer);

	/* Actor(보통 ACharacter/APawn)의 HealthComponent를 반환 */
	UFUNCTION(BlueprintPure, Category = "U0|Skill")
	static UU0SkillComponent* FindSkillComponent(const AActor* Actor);

	/* 아래의 UFUNCTION은 SkillSet의 Attribute에 접근하기 위한 BP Accessor 함수들 */

	UFUNCTION(BlueprintCallable, Category = "U0|Skill")
	float GetConcertoEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Skill")
	float GetMaxConcertoEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Skill")
	float GetUltimateEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Skill")
	float GetMaxUltimateEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Skill")
	float GetConcertoEnergyNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "U0|Skill")
	float GetUltimateEnergyNormalized() const;

	/* ASC와 HealthSet 초기화 */
	void InitializeWithAbilitySystem(UU0AbilitySystemComponent* InASC, const FCharacterAttributeData& CharacterAttributeData, int32 SlotIndex = INDEX_NONE);
	void UninitializeWithAbilitySystem();


	
	virtual void HandleConcertoEnergyChanged(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue);
	virtual void HandleMaxConcertoEnergyChanged(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue);
	virtual void HandleUltimateEnergyChanged(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue);
	virtual void HandleMaxUltimateEnergyChanged(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* EffectSpec, float Magnitude, float OldValue, float NewValue);


	/* HealthSet에 접근하기 위한 AbilitySystemComponent */
	UPROPERTY()
	TObjectPtr<UU0AbilitySystemComponent> AbilitySystemComponent;

	/* 캐싱된 HealthSet 레퍼런스 */
	UPROPERTY()
	TObjectPtr<const UU0HeroSkillSetBase> SkillSet;

	/* Health 변화에 따른 Delegate(Multicast) */
	UPROPERTY(BlueprintAssignable)
	FU0Skill_AttributeChanged OnConcertoEnergyChanged;

	UPROPERTY(BlueprintAssignable)
	FU0Skill_AttributeChanged OnMaxConcertoEnergyChanged;

	UPROPERTY(BlueprintAssignable)
	FU0Skill_AttributeChanged OnUltimateEnergyChanged;

	UPROPERTY(BlueprintAssignable)
	FU0Skill_AttributeChanged OnMaxUltimateEnergyChanged;
};
