// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "U0CameraMode.h"
#include "Camera/CameraComponent.h"
#include "U0CameraComponent.generated.h"

/* template forward declaration */
template <class TClass> class TSubclassOf;

/* (return type, delegate_name)*/
DECLARE_DELEGATE_RetVal(TSubclassOf<UU0CameraMode>, FU0CameraModeDelegate);

/**
 * 
 */
UCLASS()
class U0GAME_API UU0CameraComponent : public UCameraComponent
{
	GENERATED_BODY()
public:
	UU0CameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	static UU0CameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UU0CameraComponent>() : nullptr); }

	/*
	* member methods
	*/
	AActor* GetTargetActor() const { return GetOwner(); }
	void UpdateCameraModes();

	/*
	* CameraComponent interface
	*/
	virtual void OnRegister() final;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;

	/*
	* member variables
	*/
	/* 카메라의 blending 기능을 지원하는 stack */
	UPROPERTY()
	TObjectPtr<UU0CameraModeStack> CameraModeStack;

	/* 현재 CameraMode를 가져오는 Delegate */
	FU0CameraModeDelegate DetermineCameraModeDelegate;
};
