// Fill out your copyright notice in the Description page of Project Settings.


#include "U0Character.h"
#include "U0PawnExtensionComponent.h"
#include "U0Game/Camera/U0CameraComponent.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"
#include "U0HealthComponent.h"
#include "U0SkillComponent.h"
#include "U0CharacterData.h"
#include "U0Game/U0GameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0Character)
// Sets default values
AU0Character::AU0Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Tick을 비활성화
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// PawnExtensionComponent 생성
	PawnExtComponent = CreateDefaultSubobject<UU0PawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	{
		PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
		PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
	}

	// CameraComponent 생성
	{
		CameraComponent = CreateDefaultSubobject<UU0CameraComponent>(TEXT("CameraComponent"));
		CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f));
	}

	// HealthComponent 생성
	{
		HealthComponent = CreateDefaultSubobject<UU0HealthComponent>(TEXT("HealthComponent"));
	}

	// SkillComponent 생성
	{
		SkillComponent = CreateDefaultSubobject<UU0SkillComponent>(TEXT("SkillComponent"));
	}
}

void AU0Character::OnAbilitySystemInitialized()
{
	UU0AbilitySystemComponent* U0ASC = Cast<UU0AbilitySystemComponent>(GetAbilitySystemComponent());
	check(U0ASC);

	const UU0CharacterData* CharacterData = PawnExtComponent->GetPawnData<UU0CharacterData>();
	
	// HealthComponent의 ASC를 통한 초기화
	HealthComponent->InitializeWithAbilitySystem(U0ASC, CharacterData->PawnAttributeData, SlotIndex);

	SkillComponent->InitializeWithAbilitySystem(U0ASC, CharacterData->CharacterAttributeData, SlotIndex);

	InitializeGameplayTags();
}

void AU0Character::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeWithAbilitySystem();
	SkillComponent->UninitializeWithAbilitySystem();
}

UAbilitySystemComponent* AU0Character::GetAbilitySystemComponent() const
{
	// 앞서, 우리는 PawnExtensionComponent에 AbilitySystemComponent를 캐싱하였다
	return PawnExtComponent->GetU0AbilitySystemComponent();
}

UU0AbilitySystemComponent* AU0Character::GetU0AbilitySystemComponent() const
{
	return Cast<UU0AbilitySystemComponent>(GetAbilitySystemComponent());
}

void AU0Character::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (UU0AbilitySystemComponent* U0ASC = GetU0AbilitySystemComponent())
	{
		for (FGameplayTag Tag : U0GameplayTags::OnLandedRemoveTag)
		{
			U0ASC->RemoveLooseGameplayTag(Tag);
		}
	}
}

void AU0Character::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UU0AbilitySystemComponent* U0ASC = GetU0AbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : U0GameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				U0ASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : U0GameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				U0ASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		SetMovementModeTag(GetCharacterMovement()->MovementMode, GetCharacterMovement()->CustomMovementMode, true);
	}
}

void AU0Character::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(GetCharacterMovement()->MovementMode, GetCharacterMovement()->CustomMovementMode, true);
}

void AU0Character::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UU0AbilitySystemComponent* U0ASC = GetU0AbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = U0GameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = U0GameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			U0ASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

// Called to bind functionality to input
void AU0Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Pawn이 Possess로서, Controller와 PlayerState 정보 접근이 가능한 상태가 되었음:
	// - SetupPlayerInputComponent 확인
	PawnExtComponent->SetupPlayerInputComponent();
}

