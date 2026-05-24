// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"

// --- Collection of helper macros and types

DECLARE_LOG_CATEGORY_EXTERN(VSLog, Log, All)

// Enum associated developers 
UENUM(BlueprintType)
enum class EDeveloperName: uint8 {

	EDN_Anna				UMETA(DisplayName = "Anna Hoffstedt"),
	EDN_Isabelle			UMETA(DisplayName = "Isabelle Bostrom"),
	EDN_Jamie				UMETA(DisplayName = "Jamie Braad Sernbo"),
	EDN_Manuel				UMETA(DisplayName = "Manuel Obermayr"),
	EDN_Meike				UMETA(DisplayName = "Meike Lenting"),
	EDN_Oliver				UMETA(DisplayName = "Oliver Osterlund Stare"),
	EDN_Kalle				UMETA(DisplayName = "Kalle Schnurer"),
};