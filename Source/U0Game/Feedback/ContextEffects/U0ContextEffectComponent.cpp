// Copyright Epic Games, Inc. All Rights Reserved.


#include "U0ContextEffectComponent.h"

#include "Engine/World.h"
#include "U0ContextEffectsSubsystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0ContextEffectComponent)

class UAnimSequenceBase;
class USceneComponent;



// Sets default values for this component's properties
UU0ContextEffectComponent::UU0ContextEffectComponent()
{
	// Disable component tick, enable Auto Activate
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
	// ...
}


// Called when the game starts
void UU0ContextEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentContexts.AppendTags(DefaultEffectContexts);
	CurrentContextEffectsLibraries = DefaultContextEffectsLibraries;

	// On Begin Play, Load and Add Context Effects pairings
	if (const UWorld* World = GetWorld())
	{
		if (UU0ContextEffectsSubsystem* U0ContextEffectsSubsystem = World->GetSubsystem<UU0ContextEffectsSubsystem>())
		{
			U0ContextEffectsSubsystem->LoadAndAddContextEffectsLibraries(GetOwner(), CurrentContextEffectsLibraries);
		}
	}
}

void UU0ContextEffectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// On End PLay, remove unnecessary context effects pairings
	if (const UWorld* World = GetWorld())
	{
		if (UU0ContextEffectsSubsystem* U0ContextEffectsSubsystem = World->GetSubsystem<UU0ContextEffectsSubsystem>())
		{
			U0ContextEffectsSubsystem->UnloadAndRemoveContextEffectsLibraries(GetOwner());
		}
	}

	Super::EndPlay(EndPlayReason);
}

// Implementation of Interface's AnimMotionEffect function
void UU0ContextEffectComponent::AnimMotionEffect_Implementation(const FName Bone, const FGameplayTag MotionEffect, USceneComponent* StaticMeshComponent,
	const FVector LocationOffset, const FRotator RotationOffset, const UAnimSequenceBase* AnimationSequence,
	const bool bHitSuccess, const FHitResult HitResult, FGameplayTagContainer Contexts,
	FVector VFXScale, float AudioVolume, float AudioPitch)
{
	// Prep Components
	TArray<UAudioComponent*> AudioComponentsToAdd;
	TArray<UNiagaraComponent*> NiagaraComponentsToAdd;

	FGameplayTagContainer TotalContexts;

	// Aggregate contexts
	TotalContexts.AppendTags(Contexts);
	TotalContexts.AppendTags(CurrentContexts);

	// Check if converting Physical Surface Type to Context
	if (bConvertPhysicalSurfaceToContext)
	{
		// Get Phys Mat Type Pointer
		TWeakObjectPtr<UPhysicalMaterial> PhysicalSurfaceTypePtr = HitResult.PhysMaterial;

		// Check if pointer is okay
		if (PhysicalSurfaceTypePtr.IsValid())
		{
			// Get the Surface Type Pointer
			TEnumAsByte<EPhysicalSurface> PhysicalSurfaceType = PhysicalSurfaceTypePtr->SurfaceType;

			// If Settings are valid
			if (const UU0ContextEffectsSettings* U0ContextEffectsSettings = GetDefault<UU0ContextEffectsSettings>())
			{
				// Convert Surface Type to known
				if (const FGameplayTag* SurfaceContextPtr = U0ContextEffectsSettings->SurfaceTypeToContextMap.Find(PhysicalSurfaceType))
				{
					FGameplayTag SurfaceContext = *SurfaceContextPtr;

					TotalContexts.AddTag(SurfaceContext);
				}
			}
		}
	}

	// Cycle through Active Audio Components and cache
	for (UAudioComponent* ActiveAudioComponent : ActiveAudioComponents)
	{
		if (ActiveAudioComponent)
		{
			AudioComponentsToAdd.Add(ActiveAudioComponent);
		}
	}

	// Cycle through Active Niagara Components and cache
	for (UNiagaraComponent* ActiveNiagaraComponent : ActiveNiagaraComponents)
	{
		if (ActiveNiagaraComponent)
		{
			NiagaraComponentsToAdd.Add(ActiveNiagaraComponent);
		}
	}

	// Get World
	if (const UWorld* World = GetWorld())
	{
		// Get Subsystem
		if (UU0ContextEffectsSubsystem* U0ContextEffectsSubsystem = World->GetSubsystem<UU0ContextEffectsSubsystem>())
		{
			// Set up Audio Components and Niagara
			TArray<UAudioComponent*> AudioComponents;
			TArray<UNiagaraComponent*> NiagaraComponents;

			// Spawn effects
			U0ContextEffectsSubsystem->SpawnContextEffects(GetOwner(), StaticMeshComponent, Bone, 
				LocationOffset, RotationOffset, MotionEffect, TotalContexts,
				AudioComponents, NiagaraComponents, VFXScale, AudioVolume, AudioPitch);

			// Append resultant effects
			AudioComponentsToAdd.Append(AudioComponents);
			NiagaraComponentsToAdd.Append(NiagaraComponents);
		}
	}

	// Append Active Audio Components
	ActiveAudioComponents.Empty();
	ActiveAudioComponents.Append(AudioComponentsToAdd);

	// Append Active
	ActiveNiagaraComponents.Empty();
	ActiveNiagaraComponents.Append(NiagaraComponentsToAdd);

}

void UU0ContextEffectComponent::UpdateEffectContexts(FGameplayTagContainer NewEffectContexts)
{
	// Reset and update
	CurrentContexts.Reset(NewEffectContexts.Num());
	CurrentContexts.AppendTags(NewEffectContexts);
}

void UU0ContextEffectComponent::UpdateLibraries(
	TSet<TSoftObjectPtr<UU0ContextEffectsLibrary>> NewContextEffectsLibraries)
{
	// Clear out existing Effects
	CurrentContextEffectsLibraries = NewContextEffectsLibraries;

	// Get World
	if (const UWorld* World = GetWorld())
	{
		// Get Subsystem
		if (UU0ContextEffectsSubsystem* U0ContextEffectsSubsystem = World->GetSubsystem<UU0ContextEffectsSubsystem>())
		{
			// Load and Add Libraries to Subsystem                  
			U0ContextEffectsSubsystem->LoadAndAddContextEffectsLibraries(GetOwner(), CurrentContextEffectsLibraries);
		}
	}
}

