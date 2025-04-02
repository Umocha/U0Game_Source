// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "U0CombatSet.generated.h"

/**
 * CombatSet은 이름 그대로, 전투와 관련된 Attribute를 담고 있는 Set이다:
 * - 현재는 BaseHeal만 있지만, BaseDamage도 추가하여, 완전한 CombatSet에 필요한 AttributeSet을 정의 가능하다
 */
UCLASS()
class U0GAME_API UU0CombatSet : public UU0AttributeSet
{
	GENERATED_BODY()
public:
	UU0CombatSet();

	ATTRIBUE_ACCESSORS(UU0CombatSet, BaseHeal);
	ATTRIBUE_ACCESSORS(UU0CombatSet, BaseDamage);

	/*
	* FGameplayAttribute가 참고하는 실제 AttributeSet에 있는 데이터이다 (float보다 해당 Struct를 사용하는 것을 추천)
	* - Healing의 단위를 의미한다
	* - ex) 5.0f라면, Period당 5씩 Healing된다는 의미
	*/
	UPROPERTY(BlueprintReadOnly, Category = "U0|Combat")
	FGameplayAttributeData BaseHeal;

	UPROPERTY(BlueprintReadOnly, Category = "U0|Combat")
	FGameplayAttributeData BaseDamage;
};
