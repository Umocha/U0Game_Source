// Fill out your copyright notice in the Description page of Project Settings.


#include "U0PawnComponent_CharacterParts.h"
#include "GameFramework/Character.h"
#include "GameplayTagAssetInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0PawnComponent_CharacterParts)

//////////////////////////////////////////
// FU0CharacterPartHandle
//////////////////////////////////////////
bool FU0CharacterPartList::SpawnActorForEntry(FU0AppliedCharacterPartEntry& Entry)
{
	bool bCreatedAnyActor = false;
	// 전달된 AppliedCharacterPartEntry의 Part Class가 제대로 세팅되어 있다면
	if (Entry.Part.PartClass != nullptr)
	{
		// OwnerComponent의 Owner에 속한 World를 반환
		UWorld* World = OwnerComponent->GetWorld();

		// U0PawnComponent_CharacterParts의 Owner 어느 Component에 붙일 것인지 결정한다:
		// - GetSceneComponentToAttachTo
		if (USceneComponent* ComponentToAttachTo = OwnerComponent->GetSceneComponentToAttachTo())
		{
			// 붙일 Component인 ComponentToAttachTo의 Bone 혹은 SocketName을 통해 어디에 붙일지 Transform을 계산한다
			//   - 근데 사용을 안하고있음
			const FTransform SpawnTransform = ComponentToAttachTo->GetSocketTransform(Entry.Part.SocketName);

			// 우리는 Actor-Actor 결합이므로, ChildActorComponent를 활용한다
			UChildActorComponent* PartComponent = NewObject<UChildActorComponent>(OwnerComponent->GetOwner());
			PartComponent->SetupAttachment(ComponentToAttachTo, Entry.Part.SocketName);
			PartComponent->SetChildActorClass(Entry.Part.PartClass);
			// 참고로 RegisterComponent를 통해 마지막으로RenderWorld인 FScene에 변경 내용을 전달한다 (혹은 생성한다)
			PartComponent->RegisterComponent();

			// ChildActorComponent에서 생성한 Actor를 반환하여
			if (AActor* SpawnedActor = PartComponent->GetChildActor())
			{
				// 해당 Actor가 Parent인 U0PawnComponent_CharacterParts의 Owner Actor보다 먼저 Tick이 실행되지 않도록 선행조건을 붙인다
				if (USceneComponent* SpawnRootComponent = SpawnedActor->GetRootComponent())
				{
					SpawnRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
				}
			}
			Entry.SpawnedComponent = PartComponent;
			bCreatedAnyActor = true;
		}
	}
	return bCreatedAnyActor;
}

void FU0CharacterPartList::DestroyActorForEntry(FU0AppliedCharacterPartEntry& Entry)
{
	if (Entry.SpawnedComponent)
	{
		Entry.SpawnedComponent->DestroyComponent();
		Entry.SpawnedComponent = nullptr;
	}
}

FU0CharacterPartHandle FU0CharacterPartList::AddEntry(const FU0CharacterPart& NewPart)
{
	FU0CharacterPartHandle Result;
	Result.PartHandle = PartHandleCounter++;

	// Authority가 있다면, AppliedCharacterPartEntry를 Enrties에 추가한다
	if (ensure(OwnerComponent && OwnerComponent->GetOwner() && OwnerComponent->GetOwner()->HasAuthority()))
	{
		FU0AppliedCharacterPartEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Part = NewPart;
		NewEntry.PartHandle = Result.PartHandle;

		// 여기서 실제 Actor를 생성하고, OwnerComponent의 Owner Actor에 Actor끼리 RootComponent로 Attach 시킨다
		if (SpawnActorForEntry(NewEntry))
		{
			// Check!! 지금 당장
			// BroadcastChanged를 통해, OwnerComponent에서 Owner의 SkeletalMeshComponent를 활용하여, Animation 및 Physics를 Re-initialize 해준다
			OwnerComponent->BroadcastChanged();
		}
	}

	return Result;
}

void FU0CharacterPartList::RemoveEntry(FU0CharacterPartHandle Handle)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FU0AppliedCharacterPartEntry& Entry = *EntryIt;

		// 제거할 경우, PartHandle을 활용한다
		if (Entry.PartHandle == Handle.PartHandle)
		{
			DestroyActorForEntry(Entry);
		}
	}
}

FGameplayTagContainer FU0CharacterPartList::CollectCombinedTags() const
{
	FGameplayTagContainer Result;

	// Entries를 순회하며,
	for (const FU0AppliedCharacterPartEntry& Entry : Entries)
	{
		// Part Actor가 생성되어 SpawnedComponent에 캐싱되어 있으면
		if (Entry.SpawnedComponent)
		{
			// 해당 Actor의 IGameplayTagAssetInterface를 통해 GameplayTag를 검색한다:
			// - 현재 우리의 TaggedActor는 IGameplayTagAssetInterface를 상속받지 않으므로 그냥 넘어갈 것이다
			// - 후일 우리가 각 Part에 대해 GameplayTag를 넣고 싶다면 이걸 상속 받아 정의해야 한다:
			//    - 예로 들어, 특정 Lv100이상 장착 가능한 장비를 만들고 싶다면 넣어야겠다
			if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Entry.SpawnedComponent->GetChildActor()))
			{
				TagInterface->GetOwnedGameplayTags(Result);
			}
		}
	}

	return Result;
}


//////////////////////////////////////////
// UU0PawnComponent_CharacterParts
//////////////////////////////////////////
UU0PawnComponent_CharacterParts::UU0PawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CharacterPartList(this)
{
}
UE_DISABLE_OPTIMIZATION
USkeletalMeshComponent* UU0PawnComponent_CharacterParts::GetParentMeshComponent() const
{
	// Character를 활용하여, 최상위 SkeletalMesh를 반환한다
	if (AActor* OwnerActor = GetOwner())
	{
		if (ACharacter* OwningCharacter = Cast<ACharacter>(OwnerActor))
		{
			if (USkeletalMeshComponent* MeshComponent = OwningCharacter->GetMesh())
			{
				return MeshComponent;
			}
		}
	}
	return nullptr;
}
UE_ENABLE_OPTIMIZATION

USceneComponent* UU0PawnComponent_CharacterParts::GetSceneComponentToAttachTo() const
{
	// Parent에 SkeletalMeshComponent가 있으면 반환하고
	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		return MeshComponent;
	}

	// 그리고 RootComponent도 확인하고
	if (AActor* OwnerActor = GetOwner())
	{
		return OwnerActor->GetRootComponent();
	}

	// 그냥 nullptr 반환
	return nullptr;
}

FGameplayTagContainer UU0PawnComponent_CharacterParts::GetCombinedTags(FGameplayTag RequiredPrefix) const
{
	// 현재 장착된 CharacterPartList의 Merge된 Tags를 반환한다
	FGameplayTagContainer Result = CharacterPartList.CollectCombinedTags();
	if (RequiredPrefix.IsValid())
	{
		// 만약 GameplayTag를 통해 필터링할 경우, 필터링해서 진행한다
		return Result.Filter(FGameplayTagContainer(RequiredPrefix));
	}
	else
	{
		// 필터링할 GameplayTag가 없으면 그냥 반환
		return Result;
	}
}

void UU0PawnComponent_CharacterParts::BroadcastChanged()
{
	const bool bReinitPose = true;

	// 현재 Owner의 SkeletalMeshComponent를 반환한다
	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		// BodyMeshes를 통해, GameplayTag를 활용하여, 알맞은 SkeletalMesh로 재설정해준다
		const FGameplayTagContainer MergedTags = GetCombinedTags(FGameplayTag());
		USkeletalMesh* DesiredMesh = BodyMeshes.SelectBestBodyStyle(MergedTags);

		// SkeletalMesh를 초기화 및 Animation 초기화 시켜준다
		MeshComponent->SetSkeletalMesh(DesiredMesh, bReinitPose);

		// PhysicsAsset을 초기화 해준다
		if (UPhysicsAsset* PhysicsAsset = BodyMeshes.ForcedPhysicsAsset)
		{
			MeshComponent->SetPhysicsAsset(PhysicsAsset, bReinitPose);
		}
	}
}

FU0CharacterPartHandle UU0PawnComponent_CharacterParts::AddCharacterPart(const FU0CharacterPart& NewPart)
{
	return CharacterPartList.AddEntry(NewPart);
}

void UU0PawnComponent_CharacterParts::RemoveCharacterPart(FU0CharacterPartHandle Handle)
{
	CharacterPartList.RemoveEntry(Handle);
}
