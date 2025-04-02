// Fill out your copyright notice in the Description page of Project Settings.


#include "U0AbilitySet.h"
#include "U0AbilitySystemComponent.h"
#include "Abilities/U0GameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0AbilitySet)

///////////////////////////////////
// FU0AbilitySet_GrantedHandles
///////////////////////////////////
void FU0AbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FU0AbilitySet_GrantedHandles::TakeFromAbilitySystem(UU0AbilitySystemComponent* U0ASC)
{
	if (!U0ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			// ActivatableAbilities에서 제거한다
			// -ClearAbility() 함수를 잠깐 보고 오자
			U0ASC->ClearAbility(Handle);
		}
	}
}

///////////////////////////////////
// UU0AbilitySet
///////////////////////////////////
UU0AbilitySet::UU0AbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UU0AbilitySet::GiveToAbilitySystem(UU0AbilitySystemComponent* U0ASC, FU0AbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject, int32 InputID) const
{
	check(U0ASC);

	if (!U0ASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	// gameplay abilities를 허용:
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FU0AbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];
		if (!IsValid(AbilityToGrant.Ability))
		{
			continue;
		}

		// GiveAbility()에서 만약 EGameplayAbilityInstancingPolicy::InstancedPerActor이라면, 내부적으로 Instance를 생성하지만 그렇지 않으면 CDO를 할당한다
		// - 이를 통해 UObject 갯수를 줄여 UObject가 늘어남에 따라 늘어나는 성능적/메모리적 부하를 줄일 수 있다
		UU0GameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UU0GameplayAbility>();

		// AbilitySpec은 GiveAbility로 전달되어, ActivatableAbilities에 추가된다
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);
		AbilitySpec.InputID = InputID;

		// GiveAbility()를 잠깐 살펴보도록 하자:
		const FGameplayAbilitySpecHandle AbilitySpecHandle = U0ASC->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}
