// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API ULoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void OnTransitionTimeOutTriggered();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Transition Time Out Triggered"))
	void K2_OnTransitionTimeOutTriggered();
};
