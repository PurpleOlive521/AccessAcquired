// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType, meta = (ScriptName = "AADamageType"))
enum class EDamageType : uint8
{
	EDT_Normal		UMETA(DisplayName = "Normal"),
	EDT_Infrared	UMETA(DisplayName = "Infrared"),
	EDT_Ultraviolet UMETA(DisplayName = "Ultraviolet"),
	EDT_Gamma		UMETA(DisplayName = "Gamma"),
};

UENUM(BlueprintType)
enum class ETeam : uint8
{
	// Default response when interface is not implemented.
	ET_NotAssigned = 0	UMETA(DisplayName = "Not Assigned"),

	ET_Player = 1		UMETA(DisplayName = "Player"),
	ET_Enemy = 2		UMETA(DisplayName = "Enemy"),
	ET_Custom = 4		UMETA(DisplayName = "Custom Team"),
	ET_Max				UMETA(DisplayName = "Max Enum Value - Invalid Team")
};