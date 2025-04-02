// Fill out your copyright notice in the Description page of Project Settings.


#include "U0HealthSet.h"
#include "GameplayEffectExtension.h"
 
#include UE_INLINE_GENERATED_CPP_BY_NAME(U0HealthSet)

UU0HealthSetBase::UU0HealthSetBase()
	: Super()
{
	bOutOfHealth = false;
	MaxHealthBeforeAttributeChange = 0.0f;
	HealthBeforeAttributeChange = 0.0f;
}

void UU0HealthSetBase::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		// HealthAttribute는 [0, GetMaxHealth()]로 설정
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// MaxHealthAttribute의 최소값이 1이 보장 되도록 설정
		NewValue = FMath::Max(NewValue, 1.0f);
	}

}

void UU0HealthSetBase::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

void UU0HealthSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttribute(Attribute, NewValue);
}

bool UU0HealthSetBase::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// Handle modifying incoming normal damage
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			// TODO: U0 - DamageFromSelf 관련
			//const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(TAG_Gameplay_DamageSelfDestruct);
			//
			//if (Data.Target.HasMatchingGameplayTag(TAG_Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
			//{
			//	// Do not take away any health.
			//	Data.EvaluatedData.Magnitude = 0.0f;
			//	return false;
			//}
		}
	}

	// Save the current health
	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();

	return true;
}

void UU0HealthSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Convert into -Health and then clamp
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}
	// Healing이 업데이트 될 경우, Healing을 Health에 적용하고, Healing을 초기화해준다
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		if (!bOutOfHealth)
		{
			SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
			SetHealing(0.0f);
		}
	}
	// Health 업데이트의 경우, [0, MaxHealth]로 맞추자
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxHealthBeforeAttributeChange, GetMaxHealth());
	}

	// If health has actually changed activate callbacks
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// Check health again in case an event above changed it.
	bOutOfHealth = (GetHealth() <= 0.0f);
}

UU0HealthSet1::UU0HealthSet1()
	: Super()
	, Health_(50)
	, MaxHealth_(100)
{
}


UU0HealthSet2::UU0HealthSet2()
	: Super()
	, Health_(50)
	, MaxHealth_(100)
{
}



UU0HealthSet3::UU0HealthSet3()
	: Super()
	, Health_(50)
	, MaxHealth_(100)
{
}
