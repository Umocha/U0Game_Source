// Fill out your copyright notice in the Description page of Project Settings.


#include "U0Monster.h"
#include "U0Game/AbilitySystem/U0AbilitySystemComponent.h"
#include "U0HealthComponent.h"
#include "U0PawnData.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "U0Game/Player/U0AIController.h"
#include "BrainComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0Monster)


// Sets default values
AU0Monster::AU0Monster()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	// HealthComponent 생성
	{
		HealthComponent = CreateDefaultSubobject<UU0HealthComponent>(TEXT("HealthComponent"));
		HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
		HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
	}

	{
		OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::InitializeWithAbilitySystem));
		OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::UninitializeWithAbilitySystem));
	}
}

UAbilitySystemComponent* AU0Monster::GetAbilitySystemComponent() const
{
	return GetU0AbilitySystemComponent();
}

// Called when the game starts or when spawned
void AU0Monster::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AU0Monster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AU0Monster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// TODO : U0 - Possess되었을때 해야할 부분
}

void AU0Monster::SetPawnData(const UU0PawnData* InPawnData)
{
	// 데디케이트 고려X
	// Pawn에 대해 Authority가 없을 경우, SetPawnData는 진행하지 않음
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	// PawnData 업데이트
	PawnData = InPawnData;
}

void AU0Monster::InitializeAbilitySystem(UU0AbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC && InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		UninitializeAbilitySystem();
	}

	AActor* ExistingAvatar = InASC->GetAvatarActor();

	if ((ExistingAvatar != nullptr) && (ExistingAvatar != this))
	{

		ensure(!ExistingAvatar->HasAuthority());

		if (AU0Monster* OtherMonster = CastChecked<AU0Monster>(ExistingAvatar))
		{
			OtherMonster->UninitializeAbilitySystem();
		}
	}
	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, this);

	// OnAbilitySystemInitialized에 바인딩된 Delegate 호출
	OnAbilitySystemInitialized.Broadcast();
}

void AU0Monster::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void AU0Monster::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	// OnAbilitySystemInitialized에 UObject가 바인딩되어 있지 않으면 추가 (Uniqueness)
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	// 이미 ASC가 설정되어 있으면, Delegate를 추가로 바로 호출
	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void AU0Monster::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}

void AU0Monster::InitializeWithAbilitySystem()
{
	UU0AbilitySystemComponent* U0ASC = Cast<UU0AbilitySystemComponent>(GetAbilitySystemComponent());
	check(U0ASC);

	// HealthComponent의 ASC를 통한 초기화
	HealthComponent->InitializeWithAbilitySystem(U0ASC, PawnData->PawnAttributeData);
}

void AU0Monster::UninitializeWithAbilitySystem()
{
	HealthComponent->UninitializeWithAbilitySystem();
}

void AU0Monster::OnDeathStarted(AActor* OwningActor)
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetCharacterMovement()->StopMovementImmediately();

	AAIController* AIController = Cast<AAIController>(GetInstigatorController());

	if (AIController)
	{
		AIController->GetBrainComponent()->StopLogic(TEXT("Death"));
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

void AU0Monster::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void AU0Monster::DestroyDueToDeath()
{
	UninitAndDestroy();
}

void AU0Monster::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Controller->Destroy();
		Destroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UU0AbilitySystemComponent* U0ASC = GetU0AbilitySystemComponent())
	{
		if (U0ASC->GetAvatarActor() == this)
		{
			UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

