// Fill out your copyright notice in the Description page of Project Settings.


#include "U0AIPlayerState.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"
#include "U0Game/Character/U0PawnData.h"
#include "U0Game/AbilitySystem/U0AbilitySet.h"
#include "U0Game/AbilitySystem/Attributes/U0HealthSet.h"
#include "U0Game/AbilitySystem/Attributes/U0CombatSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0AIPlayerState)

AU0AIPlayerState::AU0AIPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UU0AbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));

	CreateDefaultSubobject<UU0CombatSet>(TEXT("CombatSet"));
	CreateDefaultSubobject<UU0HealthSet1>(TEXT("HealthSet1"));
}

void AU0AIPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	{
		FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get();
		check(ActorInfo->OwnerActor == this);
		check(ActorInfo->OwnerActor == ActorInfo->AvatarActor);
	}
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	// TODO : U0 Experience Load 관련 필요한가?
}

void AU0AIPlayerState::SetPawnData(const UU0PawnData* InPawnData)
{
	check(InPawnData);

	// PawnData가 두번 설정되는 것을 원하지 않음
	check(!PawnData);

	PawnData = InPawnData;

	// PawnData의 AbilitySets을 순회하며, ASC에 Ability를 할당(Give)한다
	// - 이 과정에서 ASC의 ActivatableAbilities에 추가된다
	for (UU0AbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
}

UAbilitySystemComponent* AU0AIPlayerState::GetAbilitySystemComponent() const
{
	return GetU0AbilitySystemComponent();
}
