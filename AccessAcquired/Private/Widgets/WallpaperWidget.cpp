// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "WallpaperWidget.h"

UWallpaperWidget::UWallpaperWidget()
{
	bAutoActivate = true;
	bAutoRestoreFocus = true;
	ActivatedVisibility = ESlateVisibility::Visible;
	DeactivatedVisibility = ESlateVisibility::Visible;
	bSetVisibilityOnActivated = true;
	bSetVisibilityOnDeactivated = true;
}

void UWallpaperWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	DeactivatedVisibility = ESlateVisibility::Visible;

	if (bHasBeenEntered)
	{
		DeactivatedVisibility = ESlateVisibility::Collapsed;
		DeactivateWidget();

		bHasBeenEntered = false;
	}
}

void UWallpaperWidget::NativeOnDeactivated()
{
	bHasBeenEntered = true;

	Super::NativeOnDeactivated();
}
