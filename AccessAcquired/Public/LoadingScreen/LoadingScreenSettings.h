// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LoadingScreenSettings.generated.h"

/**
 * Settings for the custom loading screen.
 * Allows changing of the loading screen widget, QOL settings and timing properties for designers.
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Loading Screen Settings"))
class VERTICALSLICE_API ULoadingScreenSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	// The widget to load and display as the loading screen.
	UPROPERTY(Config, EditAnywhere,meta = (MetaClass = "/Script/UMG.UserWidget"))
	TSoftClassPtr<UUserWidget> LoadingScreenWidget;

	// The ZOrder used on the widget.
	UPROPERTY(Config, EditAnywhere)
	int ZOrder = 10000;

	// How many additional seconds to keep the loading screen up after loading is finished.
	UPROPERTY(Config, EditAnywhere, meta = (ForceUnits = s))
	float HoldLoadingScreenAdditionalSecs = 2.0f;

	// The amount of time before HoldLoadingScreenAdditionalSecs elapses where we should trigger a transition out for the loading screen widget.
	UPROPERTY(Config, EditAnywhere, meta = (ForceUnits = s))
	float TransitionOutTime = 1.4f;

	UPROPERTY(Config, EditAnywhere, Category = "Debugging")
	bool bForceDisplayLoadingScreen = false;

	UPROPERTY(Config, EditAnywhere, Category = "Debugging")
	bool bLogLoadingScreenReason = false;

	UPROPERTY(Config, EditAnywhere, Category = "Debugging")
	bool bShowLoadingScreenAdditionalSecsInEditor = false;
};
