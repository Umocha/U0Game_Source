// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/Widget.h"
#include "SCircumferenceMarkerWidget.h"

#include "CircumferenceMarkerWidget.generated.h"

UCLASS()
class UCircumferenceMarkerWidget : public UWidget
{
	GENERATED_BODY()

public:
	UCircumferenceMarkerWidget(const FObjectInitializer& ObjectInitializer);

	/*
	* UWidget interfaces
	*/
	/* SWidget과 UWidget간 데이터 싱크를 맞추기 위한 메서드 */
	virtual void SynchronizeProperties() override;
	/* Widget 재생성 */
	virtual TSharedRef<SWidget> RebuildWidget() override;

	/*
	* UVisual interfaces
	*/
	/* 해당 UWidget은 MyMarkerWidget의 SWidget을 가지고 있으므로 잘 해체해야 한다 */
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

public:
	/** The list of positions/orientations to draw the markers at. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	TArray<FCircumferenceMarkerEntry> MarkerList;

	/** The radius of the circle. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (ClampMin = 0.0))
	float Radius = 48.0f;

	/** The marker image to place around the circle. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FSlateBrush MarkerImage;

	/** Whether reticle corner images are placed outside the spread radius */
	//@TODO: Make this a 0-1 float alignment instead (e.g., inside/on/outside the radius)?
	UPROPERTY(EditAnywhere, Category = Corner)
	uint8 bReticleCornerOutsideSpreadRadius : 1;

public:
	/** Sets the radius of the circle. */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetRadius(float InRadius);

private:
	/* UMG의 CircumferenceMarkerWidget에 대응되는 SWidget */
	TSharedPtr<SCircumferenceMarkerWidget> MyMarkerWidget;
};
