// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "U0PlayerCameraManager.generated.h"

/*
* Controller에 바인딩된 CameraManager
*/

#define U0_CAMERA_DEFAULT_FOV (80.0f)
#define U0_CAMERA_DEFAULT_PITCH_MIN (-89.0f)
#define U0_CAMERA_DEFAULT_PITCH_MAX (89.0f)

/**
 * 
 */
UCLASS()
class U0GAME_API AU0PlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	AU0PlayerCameraManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
