// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "WallpaperWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class VERTICALSLICE_API UWallpaperWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:

	UWallpaperWidget();

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

private:
	bool bHasBeenEntered = false;
};
