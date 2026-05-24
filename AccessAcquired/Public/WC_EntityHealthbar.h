// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "WC_EntityHealthbar.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UWC_EntityHealthbar : public UWidgetComponent
{
	GENERATED_BODY()

	// --- Begin UActorComponent Interface
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// --- End UActorComponent Interface

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsEnabled = false;

	// The max distance at which we show the widget. Outside this distance we disable the widget.
	UPROPERTY(BlueprintReadOnly)
	float MaxShownDistance = 3500.0f;
};
