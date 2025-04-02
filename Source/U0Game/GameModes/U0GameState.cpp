// Fill out your copyright notice in the Description page of Project Settings.


#include "U0GameState.h"
#include "U0Game/GameModes/U0ExperienceManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0GameState)

AU0GameState::AU0GameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ExperienceManagerComponent = CreateDefaultSubobject<UU0ExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
