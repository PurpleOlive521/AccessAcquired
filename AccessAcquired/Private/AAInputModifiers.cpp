// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AAInputModifiers.h"
#include "ExtendedGameUserSettings.h"

FInputActionValue UAAInputModifierLookInversion::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	UExtendedGameUserSettings* Settings = UExtendedGameUserSettings::Get();
	if (!Settings)
	{
		return CurrentValue;
	}

	FVector NewValue = CurrentValue.Get<FVector>();

	if (Settings->IsInvertYAxisEnabled())
	{
		NewValue.Y *= -1.0f;
	}

	if (Settings->IsInvertXAxisEnabled())
	{
		NewValue.X *= -1.0f;
	}

	return NewValue;
}

FInputActionValue UAAInputModifierLookSensitivity::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	// Don't scale boolean values
	if (CurrentValue.GetValueType() == EInputActionValueType::Boolean)
	{
		return CurrentValue;
	}

	UExtendedGameUserSettings* Settings = UExtendedGameUserSettings::Get();
	if (!Settings)
	{
		return CurrentValue;
	}

	const float Scalar = Settings->GetCameraSensitivity();

	return CurrentValue.Get<FVector>() * Scalar;
}
