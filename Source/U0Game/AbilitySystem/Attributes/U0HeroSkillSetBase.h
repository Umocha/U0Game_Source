// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "U0HeroSkillSetBase.generated.h"

struct FGameplayEffectModCallbackData;

/**
 * 
 */
UCLASS()
class U0GAME_API UU0HeroSkillSetBase : public UU0AttributeSet
{
	GENERATED_BODY()
public:
	UU0HeroSkillSetBase();


	mutable FU0AttributeEvent OnConcertoEnergyChanged;
	mutable FU0AttributeEvent OnMaxConcertoEnergyChanged;

	mutable FU0AttributeEvent OnUltimateEnergyChanged;
	mutable FU0AttributeEvent OnMaxUltimateEnergyChanged;

	virtual float GetConcertoEnergy() const { return 0; }
	virtual float GetMaxConcertoEnergy() const { return 0; }
	virtual float GetConcertoEnergyRegen() const { return 0; }
	virtual float GetUltimateEnergy() const { return 0; }
	virtual float GetMaxUltimateEnergy() const { return 0; }
	virtual float GetUltimateEnergyRegen() const { return 0; }

	virtual void SetConcertoEnergy(float NewVal) {}
	virtual void SetMaxConcertoEnergy(float NewVal) {}
	virtual void SetConcertoEnergyRegen(float NewVal) {}
	virtual void SetUltimateEnergy(float NewVal) {}
	virtual void SetMaxUltimateEnergy(float NewVal) {}
	virtual void SetUltimateEnergyRegen(float NewVal) {}


	virtual FGameplayAttribute GetConcertoEnergyAttribute() const { return FGameplayAttribute(); }
	virtual FGameplayAttribute GetMaxConcertoEnergyAttribute() const { return FGameplayAttribute(); }
	virtual FGameplayAttribute GetConcertoEnergyRegenAttribute() const { return FGameplayAttribute(); }
	virtual FGameplayAttribute GetUltimateEnergyAttribute() const { return FGameplayAttribute(); }
	virtual FGameplayAttribute GetMaxUltimateEnergyAttribute() const { return FGameplayAttribute(); }
	virtual FGameplayAttribute GetUltimateEnergyRegenAttribute() const { return FGameplayAttribute(); }


	virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const;

	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

private:
	// 변경전 상태 저장
	float ConcertoEnergyBeforeAttributeChange;
	float MaxConcertoEnergyBeforeAttributeChange;
	float UltimateEnergyBeforeAttributeChange;
	float MaxUltimateEnergyBeforeAttributeChange;
};

////////////////////////////////////////
// U0HeroSkillSet Slot
////////////////////////////////////////

UCLASS()
class UU0HeroSkillSet1 : public UU0HeroSkillSetBase
{
	GENERATED_BODY()

public:

	ATTRIBUE_ACCESSORS(UU0HeroSkillSet1, ConcertoEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet1, MaxConcertoEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet1, ConcertoEnergyRegen_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet1, UltimateEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet1, MaxUltimateEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet1, UltimateEnergyRegen_);


	virtual float GetConcertoEnergy() const { return GetConcertoEnergy_(); }
	virtual float GetMaxConcertoEnergy() const { return GetMaxConcertoEnergy_(); }
	virtual float GetConcertoEnergyRegen() const { return GetConcertoEnergyRegen_(); }
	virtual float GetUltimateEnergy() const { return GetUltimateEnergy_(); }
	virtual float GetMaxUltimateEnergy() const { return GetMaxUltimateEnergy_(); }
	virtual float GetUltimateEnergyRegen() const { return GetUltimateEnergyRegen_(); }

	virtual void SetConcertoEnergy(float NewVal) { SetConcertoEnergy_(NewVal); }
	virtual void SetMaxConcertoEnergy(float NewVal) { SetMaxConcertoEnergy_(NewVal); }
	virtual void SetConcertoEnergyRegen(float NewVal) { SetConcertoEnergyRegen_(NewVal); }
	virtual void SetUltimateEnergy(float NewVal) { SetUltimateEnergy_(NewVal); }
	virtual void SetMaxUltimateEnergy(float NewVal) { SetMaxUltimateEnergy_(NewVal); }
	virtual void SetUltimateEnergyRegen(float NewVal) { SetUltimateEnergyRegen_(NewVal); }


	virtual FGameplayAttribute GetConcertoEnergyAttribute() const { return GetConcertoEnergy_Attribute(); }
	virtual FGameplayAttribute GetMaxConcertoEnergyAttribute() const { return GetMaxConcertoEnergy_Attribute(); }
	virtual FGameplayAttribute GetConcertoEnergyRegenAttribute() const { return GetConcertoEnergyRegen_Attribute(); }
	virtual FGameplayAttribute GetUltimateEnergyAttribute() const { return GetUltimateEnergy_Attribute(); }
	virtual FGameplayAttribute GetMaxUltimateEnergyAttribute() const { return GetMaxUltimateEnergy_Attribute(); }
	virtual FGameplayAttribute GetUltimateEnergyRegenAttribute() const { return GetUltimateEnergyRegen_Attribute(); }


	/* 현재 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Concerto")
	FGameplayAttributeData ConcertoEnergy_;

	/* Max 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Concerto")
	FGameplayAttributeData MaxConcertoEnergy_;

	/* 협주 에너지 회복 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Concerto")
	FGameplayAttributeData ConcertoEnergyRegen_;

	/* 현재 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Ultimate")
	FGameplayAttributeData UltimateEnergy_;

	/* Max 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Ultimate")
	FGameplayAttributeData MaxUltimateEnergy_;

	/* 협주 에너지 회복 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Ultimate")
	FGameplayAttributeData UltimateEnergyRegen_;
};

UCLASS()
class UU0HeroSkillSet2 : public UU0HeroSkillSetBase
{
	GENERATED_BODY()

public:

	ATTRIBUE_ACCESSORS(UU0HeroSkillSet2, ConcertoEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet2, MaxConcertoEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet2, ConcertoEnergyRegen_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet2, UltimateEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet2, MaxUltimateEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet2, UltimateEnergyRegen_);


	virtual float GetConcertoEnergy() const { return GetConcertoEnergy_(); }
	virtual float GetMaxConcertoEnergy() const { return GetMaxConcertoEnergy_(); }
	virtual float GetConcertoEnergyRegen() const { return GetConcertoEnergyRegen_(); }
	virtual float GetUltimateEnergy() const { return GetUltimateEnergy_(); }
	virtual float GetMaxUltimateEnergy() const { return GetMaxUltimateEnergy_(); }
	virtual float GetUltimateEnergyRegen() const { return GetUltimateEnergyRegen_(); }

	virtual void SetConcertoEnergy(float NewVal) { SetConcertoEnergy_(NewVal); }
	virtual void SetMaxConcertoEnergy(float NewVal) { SetMaxConcertoEnergy_(NewVal); }
	virtual void SetConcertoEnergyRegen(float NewVal) { SetConcertoEnergyRegen_(NewVal); }
	virtual void SetUltimateEnergy(float NewVal) { SetUltimateEnergy_(NewVal); }
	virtual void SetMaxUltimateEnergy(float NewVal) { SetMaxUltimateEnergy_(NewVal); }
	virtual void SetUltimateEnergyRegen(float NewVal) { SetUltimateEnergyRegen_(NewVal); }


	virtual FGameplayAttribute GetConcertoEnergyAttribute() const { return GetConcertoEnergy_Attribute(); }
	virtual FGameplayAttribute GetMaxConcertoEnergyAttribute() const { return GetMaxConcertoEnergy_Attribute(); }
	virtual FGameplayAttribute GetConcertoEnergyRegenAttribute() const { return GetConcertoEnergyRegen_Attribute(); }
	virtual FGameplayAttribute GetUltimateEnergyAttribute() const { return GetUltimateEnergy_Attribute(); }
	virtual FGameplayAttribute GetMaxUltimateEnergyAttribute() const { return GetMaxUltimateEnergy_Attribute(); }
	virtual FGameplayAttribute GetUltimateEnergyRegenAttribute() const { return GetUltimateEnergyRegen_Attribute(); }


	/* 현재 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Concerto")
	FGameplayAttributeData ConcertoEnergy_;

	/* Max 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Concerto")
	FGameplayAttributeData MaxConcertoEnergy_;

	/* 협주 에너지 회복 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Concerto")
	FGameplayAttributeData ConcertoEnergyRegen_;

	/* 현재 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Ultimate")
	FGameplayAttributeData UltimateEnergy_;

	/* Max 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Ultimate")
	FGameplayAttributeData MaxUltimateEnergy_;

	/* 협주 에너지 회복 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Ultimate")
	FGameplayAttributeData UltimateEnergyRegen_;
};

UCLASS()
class UU0HeroSkillSet3 : public UU0HeroSkillSetBase
{
	GENERATED_BODY()

public:

	ATTRIBUE_ACCESSORS(UU0HeroSkillSet3, ConcertoEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet3, MaxConcertoEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet3, ConcertoEnergyRegen_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet3, UltimateEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet3, MaxUltimateEnergy_);
	ATTRIBUE_ACCESSORS(UU0HeroSkillSet3, UltimateEnergyRegen_);


	virtual float GetConcertoEnergy() const { return GetConcertoEnergy_(); }
	virtual float GetMaxConcertoEnergy() const { return GetMaxConcertoEnergy_(); }
	virtual float GetConcertoEnergyRegen() const { return GetConcertoEnergyRegen_(); }
	virtual float GetUltimateEnergy() const { return GetUltimateEnergy_(); }
	virtual float GetMaxUltimateEnergy() const { return GetMaxUltimateEnergy_(); }
	virtual float GetUltimateEnergyRegen() const { return GetUltimateEnergyRegen_(); }

	virtual void SetConcertoEnergy(float NewVal) { SetConcertoEnergy_(NewVal); }
	virtual void SetMaxConcertoEnergy(float NewVal) { SetMaxConcertoEnergy_(NewVal); }
	virtual void SetConcertoEnergyRegen(float NewVal) { SetConcertoEnergyRegen_(NewVal); }
	virtual void SetUltimateEnergy(float NewVal) { SetUltimateEnergy_(NewVal); }
	virtual void SetMaxUltimateEnergy(float NewVal) { SetMaxUltimateEnergy_(NewVal); }
	virtual void SetUltimateEnergyRegen(float NewVal) { SetUltimateEnergyRegen_(NewVal); }


	virtual FGameplayAttribute GetConcertoEnergyAttribute() const { return GetConcertoEnergy_Attribute(); }
	virtual FGameplayAttribute GetMaxConcertoEnergyAttribute() const { return GetMaxConcertoEnergy_Attribute(); }
	virtual FGameplayAttribute GetConcertoEnergyRegenAttribute() const { return GetConcertoEnergyRegen_Attribute(); }
	virtual FGameplayAttribute GetUltimateEnergyAttribute() const { return GetUltimateEnergy_Attribute(); }
	virtual FGameplayAttribute GetMaxUltimateEnergyAttribute() const { return GetMaxUltimateEnergy_Attribute(); }
	virtual FGameplayAttribute GetUltimateEnergyRegenAttribute() const { return GetUltimateEnergyRegen_Attribute(); }


	/* 현재 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Concerto")
	FGameplayAttributeData ConcertoEnergy_;

	/* Max 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Concerto")
	FGameplayAttributeData MaxConcertoEnergy_;

	/* 협주 에너지 회복 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Concerto")
	FGameplayAttributeData ConcertoEnergyRegen_;

	/* 현재 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Ultimate")
	FGameplayAttributeData UltimateEnergy_;

	/* Max 협주 에너지 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Ultimate")
	FGameplayAttributeData MaxUltimateEnergy_;

	/* 협주 에너지 회복 */
	UPROPERTY(BlueprintReadOnly, Category = "U0|Ultimate")
	FGameplayAttributeData UltimateEnergyRegen_;
};