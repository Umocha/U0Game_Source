// Fill out your copyright notice in the Description page of Project Settings.


#include "U0PlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0PlayerCameraManager)

AU0PlayerCameraManager::AU0PlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = U0_CAMERA_DEFAULT_FOV;
	ViewPitchMin = U0_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = U0_CAMERA_DEFAULT_PITCH_MAX;
}
