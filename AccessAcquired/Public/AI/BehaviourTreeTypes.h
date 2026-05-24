// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EArithmeticOperation : uint8
{
	EAO_Equal				UMETA(DisplayName = "Is Equal To"),
	EAO_NotEqual			UMETA(DisplayName = "Is Not Equal To"),
	EAO_Less				UMETA(DisplayName = "Is Less Than"),
	EAO_LessOrEqual			UMETA(DisplayName = "Is Less Than Or Equal To"),
	EAO_Greater				UMETA(DisplayName = "Is Greater Than"),
	EAO_GreaterOrEqual		UMETA(DisplayName = "Is Greater Than Or Equal To"),
};
		   