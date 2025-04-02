// Fill out your copyright notice in the Description page of Project Settings.


#include "U0EquipmentDefinition.h"
#include "U0EquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0EquipmentDefinition)

UU0EquipmentDefinition::UU0EquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 기본값으로, U0EquipmentInstance로 설정
	InstanceType = UU0EquipmentInstance::StaticClass();
}
