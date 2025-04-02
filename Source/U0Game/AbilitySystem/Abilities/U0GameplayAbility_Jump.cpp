// Fill out your copyright notice in the Description page of Project Settings.


#include "U0GameplayAbility_Jump.h"

#include "U0Game/Character/U0Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0GameplayAbility_Jump)

UU0GameplayAbility_Jump::UU0GameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UU0GameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AU0Character* U0Character = Cast<AU0Character>(ActorInfo->AvatarActor.Get());
	if (!U0Character || !U0Character->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UU0GameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UU0GameplayAbility_Jump::CharacterJumpStart()
{
	if (AU0Character* U0Character = GetU0CharacterFromActorInfo())
	{
		if (U0Character->IsLocallyControlled() && !U0Character->bPressedJump)
		{
			U0Character->UnCrouch();
			U0Character->Jump();
		}
	}
}

void UU0GameplayAbility_Jump::CharacterJumpStop()
{
	if (AU0Character* U0Character = GetU0CharacterFromActorInfo())
	{
		if (U0Character->IsLocallyControlled() && U0Character->bPressedJump)
		{
			U0Character->StopJumping();
		}
	}
}
