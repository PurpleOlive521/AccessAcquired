// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "OverheatComponent.h"
#include "OverheatWarningsWidget.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UOverheatWarningsWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UOverheatWarningsWidget() = default;

	// --- Begin UCommonUserWidget Interface
	void NativeConstruct() override;

	void NativeDestruct() override;
	// --- End UCommonUserWidget Interface

	UFUNCTION()
	void OnSeverityChanged(EOverheatSeverity Severity);

	UFUNCTION()
	void OnModeOverheated(bool bEnteredOverheat);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Activate Warning"))
	void K2_ActivateWarning();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Deactivate Warning"))
	void K2_DeactivateWarning();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Overheated"))
	void K2_OnOverheated(bool bEnteredOverheat);

protected:

	// The Severity at or above where we activate the widget.
	UPROPERTY(EditAnywhere)
	EOverheatSeverity ActivationSeverity = EOverheatSeverity::EOS_None;

	TWeakObjectPtr<UOverheatComponent> OverheatComponent = nullptr;

	bool bIsWarningActive = false;
};
