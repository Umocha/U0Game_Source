// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"

#include "U0AttributeSet.generated.h"

struct FGameplayEffectSpec;

/**
 * 아래 매크로는 AttributeSet에 Attribute를 추가할 때, 선언 및 정의해야할 메서드에 대한 간략버전을 제공한다:
 * 
 * ATTRIBUE_ACCESSORS(U0HealthSet, Health):
 * 이는 아래의 메서드를 선언 및 정의해준다
 * 
 * static FGameplayAttribute GetHealthAttribute() {...}
 * float GetHealth() const {...}
 * void SetHealth(float NewVal) {...}
 * void InitHealth(float NewVal) {...}
 */
#define ATTRIBUE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) 


 /**
  * Delegate used to broadcast attribute events, some of these parameters may be null on clients:
  * @param EffectInstigator	The original instigating actor for this event
  * @param EffectCauser		The physical actor that caused the change
  * @param EffectSpec		The full effect spec for this change
  * @param EffectMagnitude	The raw magnitude, this is before clamping
  * @param OldValue			The value of the attribute before it was changed
  * @param NewValue			The value after it was changed
 */
DECLARE_MULTICAST_DELEGATE_SixParams(FU0AttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

 /**
  * U0AttributeSet
  * - Lyra와 마찬가지로 U0에서 메인 Attribute Set Class이다
  */
UCLASS()
class U0GAME_API UU0AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:	
	UU0AttributeSet();
};
