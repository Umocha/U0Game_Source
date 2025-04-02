// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "U0HealthSet.generated.h"

struct FGameplayEffectModCallbackData;

/**
 * 
 */
UCLASS(Abstract)
class U0GAME_API UU0HealthSetBase : public UU0AttributeSet
{
	GENERATED_BODY()
public:
	UU0HealthSetBase();

	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FU0AttributeEvent OnHealthChanged;

	// Delegate when max health changes
	mutable FU0AttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FU0AttributeEvent OnOutOfHealth;

	virtual float GetHealth() const { return 0; }
	virtual float GetMaxHealth() const { return 0; }
	virtual float GetHealing() const { return 0; }
	virtual float GetDamage() const { return 0; }

	virtual void SetHealth(float NewVal) { }
	virtual void SetMaxHealth(float NewVal) { }
	virtual void SetHealing(float NewVal) { }
	virtual void SetDamage(float NewVal) { }


	virtual FGameplayAttribute GetHealthAttribute() const { return FGameplayAttribute(); }
	virtual FGameplayAttribute GetMaxHealthAttribute() const { return FGameplayAttribute(); }
	virtual FGameplayAttribute GetHealingAttribute() const { return FGameplayAttribute(); }
	virtual FGameplayAttribute GetDamageAttribute() const { return FGameplayAttribute(); }

	/*
	* Attribute의 값을 ClampAttribute()를 활용하여, 값의 범위를 유지시켜주기 위해
	* PreAttributeBaseChange와 PreAttributeChange 오버라이드
	*/
	virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/*
	* GameplayEffect가 HealthSet의 Attribute를 수정하기 전에 불리는 콜백함수:
	* - 이는 AttributeSet의 주석에도 잘 나옴, Healing이 업데이트되면, Health를 Healing을 적용하여 업데이트 가능하다
	*/
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;


private:
	// health가 0에 도달하는 시점을 추적하는데 사용
	bool bOutOfHealth;

	// 변경전 상태 저장
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;

};

/////////////////////////////////////////////////
// HealthSet Slot
/////////////////////////////////////////////////

UCLASS()
class UU0HealthSet1 : public UU0HealthSetBase
{
	GENERATED_BODY()

public:
	UU0HealthSet1();

	ATTRIBUE_ACCESSORS(UU0HealthSet1, Health_);
	ATTRIBUE_ACCESSORS(UU0HealthSet1, MaxHealth_);
	ATTRIBUE_ACCESSORS(UU0HealthSet1, Healing_);
	ATTRIBUE_ACCESSORS(UU0HealthSet1, Damage_);


	virtual float GetHealth() const override { return GetHealth_(); }
	virtual float GetMaxHealth() const override { return GetMaxHealth_(); }
	virtual float GetHealing() const override { return GetHealing_(); }
	virtual float GetDamage() const override { return GetDamage_(); }


	virtual void SetHealth(float NewVal)  override { SetHealth_(NewVal); }
	virtual void SetMaxHealth(float NewVal)  override { SetMaxHealth_(NewVal); }
	virtual void SetHealing(float NewVal)  override { SetHealing_(NewVal); }
	virtual void SetDamage(float NewVal)  override { SetDamage_(NewVal); }



	virtual FGameplayAttribute GetHealthAttribute() const override { return GetHealth_Attribute(); }
	virtual FGameplayAttribute GetMaxHealthAttribute() const override { return GetMaxHealth_Attribute(); }
	virtual FGameplayAttribute GetHealingAttribute() const override { return GetHealing_Attribute(); }
	virtual FGameplayAttribute GetDamageAttribute() const override { return GetDamage_Attribute(); }


	/* 현재 체력 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData Health_;

	/* Max 체력 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData MaxHealth_;

	/* 체력 회복치 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData Healing_;

	/* 체력 감소치 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData Damage_;
};

UCLASS()
class UU0HealthSet2 : public UU0HealthSetBase
{
	GENERATED_BODY()

public:
	UU0HealthSet2();


	ATTRIBUE_ACCESSORS(UU0HealthSet2, Health_);
	ATTRIBUE_ACCESSORS(UU0HealthSet2, MaxHealth_);
	ATTRIBUE_ACCESSORS(UU0HealthSet2, Healing_);
	ATTRIBUE_ACCESSORS(UU0HealthSet2, Damage_);


	virtual float GetHealth() const override { return GetHealth_(); }
	virtual float GetMaxHealth() const override { return GetMaxHealth_(); }
	virtual float GetHealing() const override { return GetHealing_(); }
	virtual float GetDamage() const override { return GetDamage_(); }


	virtual void SetHealth(float NewVal)  override { SetHealth_(NewVal); }
	virtual void SetMaxHealth(float NewVal)  override { SetMaxHealth_(NewVal); }
	virtual void SetHealing(float NewVal)  override { SetHealing_(NewVal); }
	virtual void SetDamage(float NewVal)  override { SetDamage_(NewVal); }



	virtual FGameplayAttribute GetHealthAttribute() const override { return GetHealth_Attribute(); }
	virtual FGameplayAttribute GetMaxHealthAttribute() const override { return GetMaxHealth_Attribute(); }
	virtual FGameplayAttribute GetHealingAttribute() const override { return GetHealing_Attribute(); }
	virtual FGameplayAttribute GetDamageAttribute() const override { return GetDamage_Attribute(); }


	/* 현재 체력 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData Health_;

	/* Max 체력 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData MaxHealth_;

	/* 체력 회복치 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData Healing_;

	/* 체력 감소치 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData Damage_;
};

UCLASS()
class UU0HealthSet3 : public UU0HealthSetBase
{
	GENERATED_BODY()

public:
	UU0HealthSet3();


	ATTRIBUE_ACCESSORS(UU0HealthSet3, Health_);
	ATTRIBUE_ACCESSORS(UU0HealthSet3, MaxHealth_);
	ATTRIBUE_ACCESSORS(UU0HealthSet3, Healing_);
	ATTRIBUE_ACCESSORS(UU0HealthSet3, Damage_);


	virtual float GetHealth() const override { return GetHealth_(); }
	virtual float GetMaxHealth() const override { return GetMaxHealth_(); }
	virtual float GetHealing() const override { return GetHealing_(); }
	virtual float GetDamage() const override { return GetDamage_(); }


	virtual void SetHealth(float NewVal)  override { SetHealth_(NewVal); }
	virtual void SetMaxHealth(float NewVal)  override { SetMaxHealth_(NewVal); }
	virtual void SetHealing(float NewVal)  override { SetHealing_(NewVal); }
	virtual void SetDamage(float NewVal)  override { SetDamage_(NewVal); }



	virtual FGameplayAttribute GetHealthAttribute() const override { return GetHealth_Attribute(); }
	virtual FGameplayAttribute GetMaxHealthAttribute() const override { return GetMaxHealth_Attribute(); }
	virtual FGameplayAttribute GetHealingAttribute() const override { return GetHealing_Attribute(); }
	virtual FGameplayAttribute GetDamageAttribute() const override { return GetDamage_Attribute(); }


	/* 현재 체력 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData Health_;

	/* Max 체력 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData MaxHealth_;

	/* 체력 회복치 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData Healing_;

	/* 체력 감소치 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Health")
	FGameplayAttributeData Damage_;
};