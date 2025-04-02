// Fill out your copyright notice in the Description page of Project Settings.


#include "U0WeaponUserInterface.h"
#include "U0Game/Equipment/U0EquipmentManagerComponent.h"
#include "U0Game/Weapons/U0WeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0WeaponUserInterface)

UU0WeaponUserInterface::UU0WeaponUserInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UU0WeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Pawn을 가져오고
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		// EquipmentManagerComponent를 활용하여, WeaponInstance를 가져오자
		if (UU0EquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UU0EquipmentManagerComponent>())
		{
			if (UU0WeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<UU0WeaponInstance>())
			{
				if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
				{
					// 새로 업데이트 해주고, OnWeaponChanged 호출 진행
					UU0WeaponInstance* OldWeapon = CurrentInstance;
					CurrentInstance = NewInstance;
					OnWeaponChanged(OldWeapon, CurrentInstance);
				}
			}
		}
	}
}
