#pragma once

#include "GameplayEffectTypes.h"

#include "U0GameplayEffectContext.generated.h"

class AActor;


USTRUCT(BlueprintType)
struct FU0GameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	FU0GameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	FU0GameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	/** Returns the wrapped FLyraGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	static U0GAME_API FU0GameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);


	virtual FGameplayEffectContext* Duplicate() const override
	{
		FU0GameplayEffectContext* NewContext = new FU0GameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FU0GameplayEffectContext::StaticStruct();
	}


	const float& GetFloatValue() const
	{
		return FloatValue;
	}

	float FloatValue = 0.0f;
};