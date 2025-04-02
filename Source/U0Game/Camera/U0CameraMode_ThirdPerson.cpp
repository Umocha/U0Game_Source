// Fill out your copyright notice in the Description page of Project Settings.


#include "U0CameraMode_ThirdPerson.h"
#include "Curves/CurveVector.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0CameraMode_ThirdPerson)

float UU0CameraMode_ThirdPerson::ZoomLevel = 1.0f;
float UU0CameraMode_ThirdPerson::MinZoom = -700.0f;
float UU0CameraMode_ThirdPerson::MaxZoom = -200.0f;
float UU0CameraMode_ThirdPerson::ZoomStep = 0.1f;

UU0CameraMode_ThirdPerson::UU0CameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UU0CameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// TargetOffsetCurve가 오버라이드되어 있다면, Curve에 값을 가져와서 적용 진행
	// - Camera 관점에서 Character의 어느 부분에 Target으로 할지 결정하는 것으로 이해하면 됨
	if (TargetOffsetCurve)
	{
		FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
		TargetOffset.X = FMath::Clamp(TargetOffset.X / ZoomLevel, MinZoom, MaxZoom);
		View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
	}

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	AActor* TargetActor = GetTargetActor();
	QueryParams.AddIgnoredActor(TargetActor);
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult, TargetActor->GetActorLocation(), View.Location,
		ECC_Camera, QueryParams
	);
	
	if (bHit)
	{
		View.Location = HitResult.Location + (HitResult.Normal * 5); // 벽 앞쪽으로 이동
	}
}

void UU0CameraMode_ThirdPerson::HandleCameraZoomInOut(float InputZoomValue)
{
	float ZoomOffset = InputZoomValue > 0 ? ZoomStep : -ZoomStep;
	ZoomLevel = FMath::Clamp(ZoomLevel + ZoomOffset, 1, 4);
}
