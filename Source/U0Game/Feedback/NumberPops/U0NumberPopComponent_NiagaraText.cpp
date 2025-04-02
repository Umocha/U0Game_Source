// Copyright Epic Games, Inc. All Rights Reserved.

#include "U0NumberPopComponent_NiagaraText.h"

#include "U0NumberPopComponent.h"
#include "U0DamagePopStyleNiagara.h"
#include "U0Game/U0LogChannels.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0NumberPopComponent_NiagaraText)

UU0NumberPopComponent_NiagaraText::UU0NumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{


}

void UU0NumberPopComponent_NiagaraText::AddNumberPop(const FU0NumberPopRequest& NewRequest)
{
	int32 LocalDamage = NewRequest.NumberToDisplay;

	//Change Damage to negative to differentiate Critial vs Normal hit
	if (NewRequest.bIsCriticalDamage)
	{
		LocalDamage *= -1;
	}

	//Add a NiagaraComponent if we don't already have one
	if (!NiagaraComp)
	{
		NiagaraComp = NewObject<UNiagaraComponent>(GetOwner());
		if (Style != nullptr)
		{
			NiagaraComp->SetAsset(Style->TextNiagara);
			NiagaraComp->bAutoActivate = false;
			
		}
		NiagaraComp->SetupAttachment(nullptr);
		check(NiagaraComp);
		NiagaraComp->RegisterComponent();
	}


	NiagaraComp->Activate(false);
	NiagaraComp->SetWorldLocation(NewRequest.WorldLocation);

	UE_LOG(LogU0, Log, TEXT("DamageHit location : %s"), *(NewRequest.WorldLocation.ToString()));
	//Add Damage information to the current Niagara list - Damage informations are packed inside a FVector4 where XYZ = Position, W = Damage
	TArray<FVector4> DamageList = UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector4(NiagaraComp, Style->NiagaraArrayName);
	DamageList.Add(FVector4(NewRequest.WorldLocation.X, NewRequest.WorldLocation.Y, NewRequest.WorldLocation.Z, LocalDamage));
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(NiagaraComp, Style->NiagaraArrayName, DamageList);
	
}

