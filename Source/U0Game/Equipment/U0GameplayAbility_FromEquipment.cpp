// Fill out your copyright notice in the Description page of Project Settings.


#include "U0GameplayAbility_FromEquipment.h"
#include "U0EquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0GameplayAbility_FromEquipment)

UU0EquipmentInstance* UU0GameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
    // CurrentActorInfo의 AbilitySystemComponent와 CurrentSpecHandle을 활용하여, GameplayAbilitySpec을 가져옴:
    // - CurrentSpecHandle은 SetCurrentActorInfo() 호출할 때, Handle 값을 받아서 저장됨:
    // - CurrentSpecHandle와 CurrentActorInfo은 같이 함
    // - FindAbilitySpecFromHandle은 GiveAbility로 허용된 ActivaleAbilities를 순회하여 GameplayAbilitySpec을 찾아냄
    if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
    {
        // GameplayAbility_FromEquipment는 EquipmentInstance로 부터 GiveAbility를 진행했으므로, SourceObject에 EquipmentInstance가 저장되어 있다
        return Cast<UU0EquipmentInstance>(Spec->SourceObject.Get());
    }

    return nullptr;
}
