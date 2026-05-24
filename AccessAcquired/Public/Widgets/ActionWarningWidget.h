// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageEventTypes.h"
#include "ActionWarningWidget.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UActionWarningWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "Play Action Warning"))
	void K2_PlayActionWarning(EDamageType Mode, bool bIsUnparryable = false);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "Stop Action Warning"))
	void K2_StopActionWarning();
};
