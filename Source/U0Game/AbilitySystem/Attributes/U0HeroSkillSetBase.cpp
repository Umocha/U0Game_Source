// Fill out your copyright notice in the Description page of Project Settings.


#include "U0HeroSkillSetBase.h"
#include "GameplayEffectExtension.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(U0HeroSkillSetBase)

UU0HeroSkillSetBase::UU0HeroSkillSetBase()
	: Super()
{
}

void UU0HeroSkillSetBase::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetConcertoEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxConcertoEnergy());
	}
	else if (Attribute == GetMaxConcertoEnergyAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetUltimateEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxUltimateEnergy());
	}
	else if (Attribute == GetMaxUltimateEnergyAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void UU0HeroSkillSetBase::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UU0HeroSkillSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UU0HeroSkillSetBase::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	if (Attribute == GetConcertoEnergyAttribute())
	{
		OnConcertoEnergyChanged.Broadcast(nullptr, nullptr, nullptr, 0, OldValue, NewValue);
	}
}

bool UU0HeroSkillSetBase::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	ConcertoEnergyBeforeAttributeChange = GetConcertoEnergy();
	MaxConcertoEnergyBeforeAttributeChange = GetMaxConcertoEnergy();
	UltimateEnergyBeforeAttributeChange = GetUltimateEnergy();
	MaxUltimateEnergyBeforeAttributeChange = GetMaxUltimateEnergy();

	return true;
}

void UU0HeroSkillSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumEnergy = 0.0f;

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetConcertoEnergyRegenAttribute())
	{
		// Convert into -Health and then clamp
		SetConcertoEnergy(FMath::Clamp(GetConcertoEnergy() + GetConcertoEnergyRegen(), MinimumEnergy, GetMaxConcertoEnergy()));
		SetConcertoEnergyRegen(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetUltimateEnergyRegenAttribute())
	{
		SetUltimateEnergy(FMath::Clamp(GetUltimateEnergy() + GetUltimateEnergyRegen(), MinimumEnergy, GetMaxUltimateEnergy()));
		SetUltimateEnergyRegen(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetConcertoEnergyAttribute())
	{
		SetConcertoEnergy(FMath::Clamp(GetConcertoEnergy(), MinimumEnergy, GetMaxConcertoEnergy()));
	}
	else if (Data.EvaluatedData.Attribute == GetUltimateEnergyAttribute())
	{
		SetUltimateEnergy(FMath::Clamp(GetUltimateEnergy(), MinimumEnergy, GetMaxUltimateEnergy()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxConcertoEnergyAttribute())
	{
		OnMaxConcertoEnergyChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxConcertoEnergyBeforeAttributeChange, GetMaxConcertoEnergy());
	}
	else if (Data.EvaluatedData.Attribute == GetMaxUltimateEnergyAttribute())
	{
		OnMaxUltimateEnergyChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxUltimateEnergyBeforeAttributeChange, GetMaxUltimateEnergy());
	}

	// If health has actually changed activate callbacks
	if (GetConcertoEnergy() != ConcertoEnergyBeforeAttributeChange)
	{
		OnConcertoEnergyChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ConcertoEnergyBeforeAttributeChange, GetConcertoEnergy());
	}
	else if (GetUltimateEnergy() != UltimateEnergyBeforeAttributeChange)
	{
		OnUltimateEnergyChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, UltimateEnergyBeforeAttributeChange, GetUltimateEnergy());
	}

}
