
#include "U0GameplayEffectContext.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(U0GameplayEffectContext)

U0GAME_API FU0GameplayEffectContext* FU0GameplayEffectContext::ExtractEffectContext(FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FU0GameplayEffectContext::StaticStruct()))
	{
		return (FU0GameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}
