// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0CameraMode.h"
#include "U0CameraMode_ThirdPerson.generated.h"

class UCurveVector;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class U0GAME_API UU0CameraMode_ThirdPerson : public UU0CameraMode
{
	GENERATED_BODY()
public:
	UU0CameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	* UU0CameraMode's interface
	*/
	virtual void UpdateView(float DeltaTime) override;

	
	static void HandleCameraZoomInOut(float InputZoomValue);
	
	
	/*
	* member variables
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const UCurveVector> TargetOffsetCurve;

	static float ZoomLevel;
	static float MinZoom;   // 최소 줌
	static float MaxZoom;   // 최대 줌
	static float ZoomStep;   // 줌 변화량
};
