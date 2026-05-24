// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputModifiers.h"
#include "AAInputModifiers.generated.h"

// Inverts the X-axis and Y-axis respectively based on game settings.
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "AA Look Inversion"))
class UAAInputModifierLookInversion : public UInputModifier
{
	GENERATED_BODY()
	
public:

	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};

// Scales the input by the camera sensitivity game setting. Does not work on boolean values!
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "AA Look Sensitivity"))
class UAAInputModifierLookSensitivity : public UInputModifier
{
	GENERATED_BODY()

public:
	
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};