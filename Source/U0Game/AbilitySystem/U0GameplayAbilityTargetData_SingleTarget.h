#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "U0GameplayAbilityTargetData_SingleTarget.generated.h"

USTRUCT()
struct FU0GameplayAbilityTargetData_SingleTargetHit : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()
public:
	FU0GameplayAbilityTargetData_SingleTargetHit()
		: CartridgeID(-1)
	{}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FU0GameplayAbilityTargetData_SingleTargetHit::StaticStruct();
	}

	/* 탄약 ID (카트리지) */
	UPROPERTY()
	int32 CartridgeID;
};