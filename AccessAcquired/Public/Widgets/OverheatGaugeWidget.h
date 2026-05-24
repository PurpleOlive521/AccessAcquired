// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplaySystemComponent.h"
#include "ReceiveDamageEventInterface.h"
#include "OverheatGaugeWidget.generated.h"

class UModeComponent;
class UOverheatComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UOverheatGaugeWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UOverheatGaugeWidget() = default;
	
	// --- Begin UCommonUserWidget Interface
	void NativePreConstruct() override;

	void NativeConstruct() override;

	void NativeDestruct() override;
	// --- End UCommonUserWidget Interface

	// Called when entering or exiting the TargetMode.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Mode Switch"))
	void K2_OnModeSwitch(bool bSwitchedTo);

	// NewPercentage is a 0.0 to 1.0 value.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Overheat Changed"))
	void K2_OnOverheatChanged(float NewPercentage);

	// NewThreshold is a 0.0 to 1.0 value.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Overclock Threshold Changed"))
	void K2_OnOverclockThresholdChanged(float NewThreshold);

	// Called when entering or exiting Overclock.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Overclocked"))
	void K2_OnOverclocked(bool bIsOverclocked);

	void ListenForOverheat(EAttributeType OverheatType);

	void ListenForOverclockThreshold(EAttributeType Attribute);
	
	UFUNCTION()
	void ListenForOverclock(bool bIsOverclocked);
	
	UFUNCTION()
	void ListenForModeSwitch(EDamageType Mode);

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UGameplaySystemComponent> GameplaySystem;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UOverheatComponent> OverheatComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDamageType TargetMode;

	EAttributeType TargetOverheatAttr = EAttributeType::EAT_NONE;

	bool bIsTargetModeActive = false;

	TWeakObjectPtr<UModeComponent> BoundModeComponent = nullptr;
};
