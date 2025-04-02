// Fill out your copyright notice in the Description page of Project Settings.


#include "U0EquipmentManagerComponent.h"
#include "U0EquipmentInstance.h"
#include "U0EquipmentDefinition.h"
#include "AbilitySystemGlobals.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0EquipmentManagerComponent)

/////////////////////////////////////
// FU0EquipmentList
/////////////////////////////////////
UU0EquipmentInstance* FU0EquipmentList::AddEntry(TSubclassOf<UU0EquipmentDefinition> EquipmentDefinition)
{
	UU0EquipmentInstance* Result = nullptr;
	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	// EquipmentDefinition의 멤버 변수들은 EditDefaultsOnly로 정의되어 있어 GetDefault로 들고 와도 우리에게 필요한 것들이 모두 들어있다
	const UU0EquipmentDefinition* EquipmentCDO = GetDefault<UU0EquipmentDefinition>(EquipmentDefinition);
	
	TSubclassOf<UU0EquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (!InstanceType)
	{
		InstanceType = UU0EquipmentInstance::StaticClass();
	}

	//Entries에 추가해주자
	FU0AppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UU0EquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
	Result = NewEntry.Instance;

	UU0AbilitySystemComponent* ASC = GetAbilitySystemComponent();
	check(ASC);
	{
		for (const TObjectPtr<UU0AbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, &NewEntry.GrantedHandles, Result);
		}
	}

	// ActorsToSpawn을 통해, Actor들을 인스턴스화 해주자
	// - 어디에? EquipmentInstance에!
	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

	return Result;
}

void FU0EquipmentList::RemoveEntry(UU0EquipmentInstance* Instance)
{
	// 단순히 그냥 Entries를 순회하며, Instance를 찾아서
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FU0AppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			UU0AbilitySystemComponent* ASC = GetAbilitySystemComponent();
			check(ASC);
			{
				// TakeFromAbilitySystem은 GiveToAbilitySystem의 반대로 역할로, ActivatableAbilities에서 제거한다
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			// Actor 제거 작업 및 Iterator를 통한 안전하게 Array에서 제거 진행
			Instance->DestroyEquipmentActors();
			EntryIt.RemoveCurrent();
		}
	}
}

UU0AbilitySystemComponent* FU0EquipmentList::GetAbilitySystemComponent()
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();

	// GetAbilitySystemComponentFromActor를 잠시 확인해보자:
	// - EquipmentManagerComponent는 AU0Character를 Owner로 가지고 있다
	// - 해당 함수는 IAbilitySystemInterface를 통해 AbilitySystemComponent를 반환하다
	// - 우리는 U0Character에 IAbilitySystemInterface를 상속받을 필요가 있다
	return Cast<UU0AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

/////////////////////////////////////
// UU0EquipmentManagerComponent
/////////////////////////////////////
UU0EquipmentManagerComponent::UU0EquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
}

UU0EquipmentInstance* UU0EquipmentManagerComponent::EquipItem(TSubclassOf<UU0EquipmentDefinition> EquipmentDefinition)
{
	UU0EquipmentInstance* Result = nullptr;
	if (EquipmentDefinition)
	{
		Result = EquipmentList.AddEntry(EquipmentDefinition);
		if (Result)
		{
			Result->OnEquipped();
		}
	}
	return Result;
}

void UU0EquipmentManagerComponent::UnequipItem(UU0EquipmentInstance* ItemInstance)
{
	if (ItemInstance)
	{
		// 해당 함수는 BP의 Event노드를 호출해준다 (자세한건 해당 함수 구현하면서 보자)
		ItemInstance->OnUnequipped();

		// EquipmentList에 제거해준다:
		// - 제거하는 과정을 통해 추가되었던 Actor Instance를 제거를 진행한다
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

UU0EquipmentInstance* UU0EquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UU0EquipmentInstance> InstanceType)
{
	for (FU0AppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UU0EquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr; 
}

TArray<UU0EquipmentInstance*> UU0EquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UU0EquipmentInstance> InstanceType) const
{
	TArray<UU0EquipmentInstance*> Results;

	// EquipmentList를 순회하며
	for (const FU0AppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UU0EquipmentInstance* Instance = Entry.Instance)
		{
			// InstanceType에 맞는 Class이면 Results에 추가하여 반환
			// - 우리의 경우, U0RangedWeaponInstance가 될거임
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}

	return Results;
}

