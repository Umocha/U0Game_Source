// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpec.h"
#include "U0AbilitySet.generated.h"

class UU0GameplayAbility;
class UU0AbilitySystemComponent;

/*
* GameplayAbility의 Wrapper 클래스 
* - 추가적인 커스터마이징이 가능함
*/
USTRUCT(BlueprintType)
struct FU0AbilitySet_GameplayAbility
{
	GENERATED_BODY()

	/* 허용된 GameplayAbility */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UU0GameplayAbility> Ability = nullptr;

	/* Input 처리를 위한 GameplayTag */
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;

	/* Ability의 허용 조건 (Level) */
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

};

USTRUCT()
struct FU0AbilitySet_GrantedHandles
{
	GENERATED_BODY()

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void TakeFromAbilitySystem(UU0AbilitySystemComponent* U0ASC);

protected:
	/* 허용된 GameplayAbilitySpecHandle(int32) */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};

/**
 * 
 */
UCLASS()
class U0GAME_API UU0AbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UU0AbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* ASC에 허용가능한 Ability를 추가한다 */
	void GiveToAbilitySystem(UU0AbilitySystemComponent* U0ASC, FU0AbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr, int32 InputID = INDEX_NONE) const;

	/* 허용된 GameplayAbilities */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FU0AbilitySet_GameplayAbility> GrantedGameplayAbilities;
};
