// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType, meta = (ScriptName = "AADamageType"))
enum class EDamageType : uint8
{
	EDT_Normal		UMETA(DisplayName = "Normal"),
	EDT_Infrared	UMETA(DisplayName = "Infrared"),
	EDT_Ultraviolet UMETA(DisplayName = "Ultraviolet"),
	EDT_Gamma		UMETA(DisplayName = "Gamma"),

	ENUM_MAX		UMETA(DisplayName = "Max Enum Value - Invalid DamageType")
};

namespace CombatConstants
{
	const TArray<EDamageType> AllModes = { EDamageType::EDT_Normal, EDamageType::EDT_Infrared, EDamageType::EDT_Ultraviolet, EDamageType::EDT_Gamma };
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