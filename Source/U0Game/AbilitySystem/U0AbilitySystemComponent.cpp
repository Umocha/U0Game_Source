// Fill out your copyright notice in the Description page of Project Settings.


#include "U0AbilitySystemComponent.h"
#include "Abilities/U0GameplayAbility.h"
#include "U0Game/Animation/U0AnimInstance.h"
#include "U0Game/AbilitySystem/Attributes/U0HealthSet.h"
#include "U0Game/AbilitySystem/Attributes/U0HeroSkillSetBase.h"
#include "U0AbilityTagRelationshipMapping.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0AbilitySystemComponent)

UU0AbilitySystemComponent::UU0AbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UU0AbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bHasNewPawnAvatar)
	{
		// Notify all abilities that a new pawn avatar has been set
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			UU0GameplayAbility* U0AbilityCDO = Cast<UU0GameplayAbility>(AbilitySpec.Ability);
			if (!U0AbilityCDO)
			{
				continue;
			}

			if (U0AbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
			{
				TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
				for (UGameplayAbility* AbilityInstance : Instances)
				{
					UU0GameplayAbility* U0AbilityInstance = Cast<UU0GameplayAbility>(AbilityInstance);
					if (U0AbilityInstance)
					{
						// Ability instances may be missing for replays
						U0AbilityInstance->OnPawnAvatarSet();
					}
				}
			}
			else
			{
				U0AbilityCDO->OnPawnAvatarSet();
			}
		}

		if (UU0AnimInstance* U0AnimInst = Cast<UU0AnimInstance>(ActorInfo->GetAnimInstance()))
		{
			U0AnimInst->InitializeWithAbilitySystem(this);
		}

		TryActivateAbilitiesOnSpawn();
	}
}

void UU0AbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (const UU0GameplayAbility* LyraAbilityCDO = Cast<UU0GameplayAbility>(AbilitySpec.Ability))
		{
			LyraAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
		}
	}
}

void UU0AbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UU0AbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}
void UU0AbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	FGameplayTagContainer ModifiedBlockTags = BlockTags;
	FGameplayTagContainer ModifiedCancelTags = CancelTags;

	for (const UU0AbilityTagRelationshipMapping* Mapping : TagRelationshipMappings)
	{
		// Use the mapping to expand the ability tags into block and cancel tag
		Mapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &ModifiedBlockTags, &ModifiedCancelTags);
	}

	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, ModifiedBlockTags, bExecuteCancelTags, ModifiedCancelTags);

	//@TODO: Apply any special logic like blocking input or movement
}

int32 UU0AbilitySystemComponent::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	int32 TriggeredCount = 0;
	FGameplayTag CurrentTag = EventTag;
	ABILITYLIST_SCOPE_LOCK();
	while (CurrentTag.IsValid())
	{
		if (GameplayEventTriggeredAbilities.Contains(CurrentTag))
		{
			TArray<FGameplayAbilitySpecHandle> TriggeredAbilityHandles = GameplayEventTriggeredAbilities[CurrentTag];

			for (const FGameplayAbilitySpecHandle& AbilityHandle : TriggeredAbilityHandles)
			{
				if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle))
				{
					if (Spec->InputID != INDEX_NONE && Spec->InputID != ActiveIndex)
					{
						continue;
					}

					if (TriggerAbilityFromGameplayEvent(AbilityHandle, AbilityActorInfo.Get(), EventTag, Payload, *this))
					{
						TriggeredCount++;
					}

				}
			}
		}

		CurrentTag = CurrentTag.RequestDirectParent();
	}

	if (FGameplayEventMulticastDelegate* Delegate = GenericGameplayEventCallbacks.Find(EventTag))
	{
		// Make a copy before broadcasting to prevent memory stomping
		FGameplayEventMulticastDelegate DelegateCopy = *Delegate;
		DelegateCopy.Broadcast(Payload);
	}

	// Make a copy in case it changes due to callbacks
	TArray<TPair<FGameplayTagContainer, FGameplayEventTagMulticastDelegate>> LocalGameplayEventTagContainerDelegates = GameplayEventTagContainerDelegates;
	for (TPair<FGameplayTagContainer, FGameplayEventTagMulticastDelegate>& SearchPair : LocalGameplayEventTagContainerDelegates)
	{
		if (SearchPair.Key.IsEmpty() || EventTag.MatchesAny(SearchPair.Key))
		{
			SearchPair.Value.Broadcast(EventTag, Payload);
		}
	}

	return TriggeredCount;
}

void UU0AbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	if (InputTag.IsValid())
	{
		// 허용된 GameplayAbilitySpec을 순회
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			// Ability가 존재하고, DynamicAbilityTags에 InputTag와 같은게 있을경우, Pressed/Held에 추가 Ability 처리 대기
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				// InputID에 등록된 Index가 현재 Index와 같거나 None일 경우에만 처리
				if (AbilitySpec.InputID == INDEX_NONE || AbilitySpec.InputID == ActiveIndex)
				{
					InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
					InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
				}
			}
		}
	}
}

void UU0AbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				// Released는 추가, Held는 제거
				if (AbilitySpec.InputID == INDEX_NONE || AbilitySpec.InputID == ActiveIndex)
				{
					InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
					InputHeldSpecHandles.Remove(AbilitySpec.Handle);
				}
			}
		}
	}
}
void UU0AbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// InputHeldSpceHandles에 대해 Ability 처리를 위해 AbilitiesToActivate에 추가한다
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		// FindAbilitySpecFromHandle 확인:
		// - ActivatableAbilities의 Handle 값 비교를 통해 GameplayAbilitySpec 반환
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UU0GameplayAbility* U0AbilityCDO = CastChecked<UU0GameplayAbility>(AbilitySpec->Ability);

				// ActivationPolicy가 WhileInputActive 속성이면 활성화로 등록
				if (U0AbilityCDO && U0AbilityCDO->ActivationPolicy == EU0AbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// 이미 Ability가 활성화되어 있을 경우, Input Event(InputPressed)만 호출
					// -AbilitySpecInputPressed 확인
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UU0GameplayAbility* U0AbilityCDO = CastChecked<UU0GameplayAbility>(AbilitySpec->Ability);

					// ActivationPolicy가 OnInputTriggered 속성이면 활성화로 등록
					if (U0AbilityCDO && U0AbilityCDO->ActivationPolicy == EU0AbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// 등록된 AbilitiesToActivate를 한꺼번에 등록 시작:
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		// 모든 것이 잘 진행되었다면, CallActivate 호출로 BP의 Activate 노드가 실행될 것임
		TryActivateAbility(AbilitySpecHandle);
	}

	// 이번 프레임에 Release되었다면, 관련 GameplayAbility 처리:
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
  		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
 			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;
				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UU0AbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}


void UU0AbilitySystemComponent::AddTagRelationshipMapping(UU0AbilityTagRelationshipMapping* NewMapping)
{
	TagRelationshipMappings.Add(NewMapping);
}

void UU0AbilitySystemComponent::RemoveTagRelationshipMapping(UU0AbilityTagRelationshipMapping* Mapping)
{
	TagRelationshipMappings.Remove(Mapping);
}

void UU0AbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
	for (const UU0AbilityTagRelationshipMapping* Mapping : TagRelationshipMappings)
	{
		Mapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
	}
}

void UU0AbilitySystemComponent::ActivateAbilityInSlot(int32 NewIndex)
{
	ClearAbilityInput();
	ActiveIndex = NewIndex;
}


const UU0HealthSetBase* UU0AbilitySystemComponent::GetHealthSet(int32 SlotIndex) const
{
	switch (SlotIndex)
	{
	case 1:
		return GetSet<UU0HealthSet2>();
	case 2:
		return GetSet<UU0HealthSet3>();
	case 0:
	default:
		return GetSet<UU0HealthSet1>();
	}

}

const UU0HeroSkillSetBase* UU0AbilitySystemComponent::GetSkillSet(int32 SlotIndex) const
{
	switch (SlotIndex)
	{
	case 1:
		return GetSet<UU0HeroSkillSet2>();
	case 2:
		return GetSet<UU0HeroSkillSet3>();
	case 0:
	default:
		return GetSet<UU0HeroSkillSet1>();
	}
}

void UU0AbilitySystemComponent::CancelAbilitiesWithTags(const FGameplayTagContainer& TagsToCancel)
{
	CancelAbilities(&TagsToCancel);
}
