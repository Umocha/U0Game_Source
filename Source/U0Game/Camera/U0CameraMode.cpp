// Fill out your copyright notice in the Description page of Project Settings.


#include "U0CameraMode.h"
#include "U0PlayerCameraManager.h"
#include "U0CameraComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(U0CameraMode)

/* FU0CameraModeView */
FU0CameraModeView::FU0CameraModeView()
	: Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(U0_CAMERA_DEFAULT_FOV)
{
}

void FU0CameraModeView::Blend(const FU0CameraModeView& Other, float OtherWeight)
{
	if (OtherWeight <= 0.0f)
	{
		return;
	}
	else if (OtherWeight >= 1.0f)
	{
		// Weight가 1.0이면 Other를 덮어쓰면 된다
		*this = Other;
		return;
	}

	// Location + OtherWeight * (Other.Location - Location)
	Location = FMath::Lerp(Location, Other.Location, OtherWeight);

	// Location과 같은 방식 Lerp (ControlRotation과 FieldOfView도 같음)
	const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (OtherWeight * DeltaRotation);

	const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}
 

/* CameraMode*/
UU0CameraMode::UU0CameraMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FieldOfView = U0_CAMERA_DEFAULT_FOV;
	ViewPitchMin = U0_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = U0_CAMERA_DEFAULT_PITCH_MAX;

	BlendTime = 0.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;

	BlendFunction = EU0CameraModeBlendFunction::EaseOut;
	BlendExponent = 4.0f;
}

void UU0CameraMode::UpdateCameraMode(float DeltaTime)
{
	// Actor를 활용하여, Pivot[Location|Rotation]을 계산하여, View를 업데이트
	UpdateView(DeltaTime);

	// BlendWeight를 DeltaTime을 활용하여, BlendAlpha 계산 후, BlendFunction에 맞게 재-매핑하여 최종 계산
	UpdateBlending(DeltaTime);
}

void UU0CameraMode::UpdateView(float DeltaTime)
{
	// CameraMode를 가지고 있는 CameraComponent의 Owner인 Character(Pawn)을 활용하여, PivotLocation/Rotation을 반환함
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	// Pitch 값에 대해 Min/Max를 Clamp시킴
	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	// FU0CameraModeView에 PivotLocation/Rotation 설정
	View.Location = PivotLocation;
	View.Rotation = PivotRotation;

	// PivotRotation을 똑같이 ControlRotation으로 활용
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// 정리하면, Character의 Location과 ControlRotation을 활용하여, View를 업데이트함
}

void UU0CameraMode::UpdateBlending(float DeltaTime)
{
	// BlendAlpha를 DeltaTime을 통해 계산
	if (BlendTime > 0.0f)
	{
		// BlendTime은 Blending 과정 총 시간(초)
		// - BlendAlpha는 0 -> 1로 변화는 과정:
		// - DeltaTime을 활용하여, BlendTime을 1로 볼 경우, 진행 정도를 누적
		BlendAlpha += (DeltaTime / BlendTime);
	}
	else 
	{
		BlendAlpha = 1.0f;
	}

	// BlendWeight의 [0, 1]를 BlendFunction에 맞게 재매핑
	const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;
	switch (BlendFunction)
	{
	case EU0CameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;
	case EU0CameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case EU0CameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case EU0CameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	default:
		checkf(false, TEXT("UpdateBlending: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}

UU0CameraComponent* UU0CameraMode::GetU0CameraComponent() const
{
	// 우리가 앞서 UU0CameraMode를 생성하는 곳은 UU0CameraModeStack이었다:
	// - 해당 코드를 보면, GetOuter()를 U0CameraMode로 U0CameraComponent로 설정하였다
	// - UU0CameraModeStack::GetCameraModeInstance() 확인
	return CastChecked<UU0CameraComponent>(GetOuter());
}

AActor* UU0CameraMode::GetTargetActor() const
{
	const UU0CameraComponent* U0CameraComponent = GetU0CameraComponent();
	return U0CameraComponent->GetTargetActor();
}

FVector UU0CameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// BaseEyeHeight를 고려하여, ViewLocation을 반환함
		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UU0CameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// GetViewRotation() 확인, 보통 Pawn의 ControlRotation을 반환
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

void UU0CameraMode::SetBlendWeight(float Weight)
{
	BlendWeight = FMath::Clamp(Weight, 0.0f, 1.0f);

	// Since we're setting the blend weight directly, we need to calculate the blend alpha to account for the blend function.
	const float InvExponent = (BlendExponent > 0.0f) ? (1.0f / BlendExponent) : 1.0f;

	switch (BlendFunction)
	{
	case EU0CameraModeBlendFunction::Linear:
		BlendAlpha = BlendWeight;
		break;

	case EU0CameraModeBlendFunction::EaseIn:
		BlendAlpha = FMath::InterpEaseIn(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	case EU0CameraModeBlendFunction::EaseOut:
		BlendAlpha = FMath::InterpEaseOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	case EU0CameraModeBlendFunction::EaseInOut:
		BlendAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, BlendWeight, InvExponent);
		break;

	default:
		checkf(false, TEXT("SetBlendWeight: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
		break;
	}
}


/* CameraModeStack */
UU0CameraModeStack::UU0CameraModeStack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UU0CameraMode* UU0CameraModeStack::GetCameraModeInstance(TSubclassOf<UU0CameraMode>& CameraModeClass)
{
	check(CameraModeClass);

	// CameraModeInstances에서 먼저 생성되어있는지 확인 후, 반환한다.
	for (UU0CameraMode* CameraMode : CameraModeInstances)
	{
		// CameraMode는 UClass를 비교한다:
		// - 즉, CameraMode는 클래스 타입에 하나만 생기게된다
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	// CameraModeClass에 알맞는 CameraMode의 인스턴스가 없다면 생성한다:
	UU0CameraMode* NewCameraMode = NewObject<UU0CameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	// 여기서 우리는 CameraModeInstances는 CameraModeClass에 맞는 인스턴스를 가지고(관리하는) 컨테이너라는 것을 알 수 있다
	CameraModeInstances.Add(NewCameraMode);

	return NewCameraMode;
}

void UU0CameraModeStack::PushCameraMode(TSubclassOf<UU0CameraMode>& CameraModeClass)
{
	if (!CameraModeClass)
	{
		return;
	}

	UU0CameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	int32 StackSize = CameraModeStack.Num();
	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		// CameraModeStack[0] 가장 최근에 이미 CameraMode가 Stacking되었으므로 그냥 리턴!
		return;
	}

	// ExistingStackIndex는 CameraModeStack에서 CameraMode에 맞는 Index를 찾음
	// ExistingStackContribution은 위에서 아래로 최종 BlendWeight 값을 찾기 위해 초기값으로 1.0으로 설정
	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.0f;

	/*
	* BlendWeight	|	ExistingStackContribution	|	ExistingStackContribution (accumulated)
	* 0.1f			|	(1.0f) * 0.1f = 0.1f		|	(1.0f - 0.1f) = 0.9f
	* 0.3f			|	(0.9f) * 0.3f = 0.27f		|	(1.0f - 0.3f) * 0.9f = 0.63f
	* 0.6f			|	(0.63f) * 0.6f = 0.378f		|	(1.0f - 0.6f) * 0.63f = 0.252f
	* 1.0f			|	(0.252f) * 1.0f =  0.252f	|
	*				|	0.1f + 0.27f + 0.378f + 0.252f = 1.0f
	*/
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			ExistingStackIndex = StackIndex;
			// BlendWeight를 CameraMode에 추가해주자:
			// - 여기서 ExistingStackContribution 우리가 찾는 CameraMode를 찾았으니까, 누적된 BlendWeight와 함께 Blendweight을 곱하여, 루프를 빠져나온다
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}
		else
		{
			// 당연히 우리가 원하는 CameraMode가 아니니까, InvBlendWeight = (1.0 - BlendWeight)를 곱해줘야, 값이 누적된다
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
		}
	}
	// 해당 루프의 동작 원리는 앞서 적어놓은 표를 확인하며 이해하자

	// 우리는 CameraMode를 Top으로 반영하기 위해, 당연히 중간에 있었다면, 제거하여 다시 Push 해줘야 한다
	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		StackSize--;
	}
	else
	{
		// 없었으니까 당연히 Contribution은 0으로
		ExistingStackContribution = 0.0f;
	}

	// BlendTime이 0보다 크다는 것은 Blend을 얼마 시간동안 진행함을 의미 따라서, ExistingStackContribution을 적용
	// - 따라서 Blend하지 않는다면, BlendWeight를 1.0을 넣어 새로 넣는 CameraMode만 적용할 것이다.
	const bool bShouldBlend = ((CameraMode->BlendTime > 0.0f) && (StackSize > 0));
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);

	CameraMode->SetBlendWeight(BlendWeight);

	// 흠.. Array를 Stack처럼 사용하는것은 알지만, Index 0에 넣는건 비효율적인다..
	// - Push, Pop 메서드와 같이그냥 last에 넣지 않는 이유가 있을까?
	CameraModeStack.Insert(CameraMode, 0);

	// 앞서 설명했듯이 마지막은 항상 1.0이 되어야 함!
	CameraModeStack.Last()->SetBlendWeight(1.0f);
}

void UU0CameraModeStack::EvaluateStack(float DeltaTime, FU0CameraModeView& OutCameraModeView)
{
	// Top -> Bottom [0 -> Num]까지 순차적으로 Stack에 있는 CameraMode 업데이트
	UpdateStack(DeltaTime);

	// Bottom -> Top까지 CameraModeStack에 대해 Blending 진행
	BlendStack(OutCameraModeView);
}

void UU0CameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		UU0CameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		// UpdateCameraMode를 확인해보자:
		CameraMode->UpdateCameraMode(DeltaTime);

		// 만약 하나라도 CameraMode가 BlendWeight가 1.0에 도달했다면, 그 이후 CameraMode를 제거한다
		if (CameraMode->BlendWeight >= 1.0f)
		{
			RemoveIndex = StackIndex + 1;
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveCount > 0)
	{
		// 생각해보면 이거 때문에 Pop하고 Push를 안한듯?
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}

}

void UU0CameraModeStack::BlendStack(FU0CameraModeView& OutCameraModeView) const
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	// CameraModeStack은 Bottom -> Top 순서로 Blend를 진행한다
	const UU0CameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	// 참고로 가장 아래 (Bottom)은 BlendWeight가 1.0이다!
	OutCameraModeView = CameraMode->View;

	// 이미 Index = [StackSize - 1], OutCameraModeView로 지정했으므로, [StackSize - 2]부터 순회하면 된다
	for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		// U0CameraModeView Blend 함수를 구현하자:
		OutCameraModeView.Blend(CameraMode->View, CameraMode->BlendWeight);
	}
}
