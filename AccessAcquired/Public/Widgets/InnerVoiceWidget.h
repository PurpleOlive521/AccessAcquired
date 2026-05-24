// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "BaseDialogueWidget.h"

#include "InnerVoiceWidget.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UInnerVoiceWidget : public UBaseDialogueWidget
{
	GENERATED_BODY()
	

	virtual void ResetInternals_Implementation() override;

};
