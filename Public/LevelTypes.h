// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

// To allow conversion from Level to level name FString, for easier work with artists in the Editor
UENUM(BlueprintType)
enum class ELevelName : uint8
{
	ELN_Backstreets_StartArea		UMETA(DisplayName = "Backstreets: Start Area"),
	ELN_Backstreets_Plaza			UMETA(DisplayName = "Backstreets: Plaza"),
	ELN_Backstreets_FirstFight		UMETA(DisplayName = "Backstreets: First Fight"),
	ELN_Backstreets_VioletTower		UMETA(DisplayName = "Backstreets: Violet Tower"),
	ELN_Backstreets_PowerTest		UMETA(DisplayName = "Backstreets: Power Test"),
	ELN_Mindspace					UMETA(DisplayName = "Mindspace"),

	ELN_None						UMETA(DisplayName = "None"),
};
