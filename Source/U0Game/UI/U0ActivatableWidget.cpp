// Fill out your copyright notice in the Description page of Project Settings.


#include "U0ActivatableWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(U0ActivatableWidget)

int32 UU0ActivatableWidget::MenuStack = 0;

UU0ActivatableWidget::UU0ActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UU0ActivatableWidget::GetDesiredInputConfig() const
{
	// 앞서 정의 했던 WidgetInputMode에 따라, InputConfig를 설정해준다
	// - ECommonInputMode에 따라 Input의 흐름을 Game/Menu 혹은 둘다로 정하는 것이 가능하다
	// - 예로 들어, 우리가 Menu로 들어갔을 경우, 더 이상 Game에 Input을 보내고 싶지 않을 경우 매우 용의하다:
	//   - 생각해보면, Menu모드인데 Space를 누르거나 MouseClick으로 총알이 나간다면... 
	switch (InputConfig)
	{
	case EU0WidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case EU0WidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case EU0WidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case EU0WidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}

void UU0ActivatableWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (InputConfig == EU0WidgetInputMode::Menu)
	{
		if (MenuStack++ == 0)
		{
			GetOwningPlayer()->SetInputMode(FInputModeUIOnly());
		}
	}

}

void UU0ActivatableWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (InputConfig == EU0WidgetInputMode::Menu)
	{
		
		if (--MenuStack == 0)
		{
			GetOwningPlayer()->SetInputMode(FInputModeGameAndUI());
		}
	}
}
