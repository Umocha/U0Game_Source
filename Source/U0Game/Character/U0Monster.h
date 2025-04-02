// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "U0Monster.generated.h"

class UU0PawnData;
class UU0AbilitySystemComponent;
class UU0HealthComponent;

UCLASS()
class U0GAME_API AU0Monster : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AU0Monster();

	/*
	* IAbilitySystemInterface
	*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	void SetPawnData(const UU0PawnData* InPawnData);

	UFUNCTION(BlueprintCallable, Category = "U0|Monster")
	UU0AbilitySystemComponent* GetU0AbilitySystemComponent() const { return AbilitySystemComponent; }

	/* AbilitySystemComponent의 AvatarActor 대상 초기화/해제 호출 */
	void InitializeAbilitySystem(UU0AbilitySystemComponent* InASC, AActor* InOwnerActor);
	void UninitializeAbilitySystem();

	/* OnAbilitySystem[Initialized|Uninitialzed] Delegate에 추가 */
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

	void InitializeWithAbilitySystem();
	void UninitializeWithAbilitySystem();

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	void DestroyDueToDeath();
	void UninitAndDestroy();

	/* ASC Init과 Uninit의 Delegate 추가 */
	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	/* Pawn을 생성한 데이터를 캐싱 */
	UPROPERTY(EditInstanceOnly, Category = "U0|Pawn")
	TObjectPtr<const UU0PawnData> PawnData;

	/* AbilitySystemComponent 캐싱 */
	UPROPERTY()
	TObjectPtr<UU0AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "U0|Monster")
	TObjectPtr<UU0HealthComponent> HealthComponent;
};
