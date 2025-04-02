// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "U0AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class U0GAME_API UU0AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	/*
	* UAnimInstance's interface
	*/
	virtual void NativeInitializeAnimation() override;

	/*
	* member methods
	*/
	void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

	/* 해당 속성값은 Lyra의 AnimBP에서 사용되는 값이므로 정의해주자 */
	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;

	/* GameplayTag와 AnimInstance의 속성값을 매핑해준다 */
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

};
