// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "GameplayUtilityBlueprintLibrary.h"

#include "Attribute.h"
#include "GameplayTagDefines.h"
#include "DevCommons.h"

FName UGameplayUtilityBlueprintLibrary::ConvertLevelToName(ELevelName Level)
{
	FString LevelName;

	switch (Level)
	{
	case ELevelName::ELN_Backstreets_StartArea:
		LevelName = TEXT("Backstreet_StartArea");
		break;

	case ELevelName::ELN_Backstreets_Plaza:
		LevelName = TEXT("Backstreet_Plaza");
		break;

	case ELevelName::ELN_Backstreets_FirstFight:
		LevelName = TEXT("Backstreet_FirstFight");
		break;

	case ELevelName::ELN_Backstreets_VioletTower:
		LevelName = TEXT("Backstreet_VioletTower");
		break;

	case ELevelName::ELN_Backstreets_PowerTest:
		LevelName = TEXT("Backstreet_PowerTest");
		break;

	case ELevelName::ELN_Mindspace:
		LevelName = TEXT("MindspaceLevel");
		break;

	default:
		// There should always be a corresponding type
		checkNoEntry(); 
		return FName();
	}

	return FName(LevelName);
}

FString UGameplayUtilityBlueprintLibrary::GetRandomLetter()
{
	FString Letters = TEXT("abcdefghijklmnopqrstuvwxyz");
	int32 RandomIndex = FMath::RandRange(0, Letters.Len() - 1);
	return FString(1, &Letters[RandomIndex]);
}

EAttributeType UGameplayUtilityBlueprintLibrary::GetCorrespondingWeakness(const EDamageType& DamageType)
{
	switch (DamageType)
	{
	case EDamageType::EDT_Normal:
		return EAttributeType::EAT_NormalWeakness;

	case EDamageType::EDT_Infrared:
		return EAttributeType::EAT_InfraredWeakness;

	case EDamageType::EDT_Ultraviolet:
		return EAttributeType::EAT_UltravioletWeakness;

	case EDamageType::EDT_Gamma:
		return EAttributeType::EAT_GammaWeakness;

	default:
		// There should always be a corresponding type
		checkNoEntry();
		return EAttributeType();
	}
}

FGameplayTag UGameplayUtilityBlueprintLibrary::GetModeAsGameplayTag(const EDamageType& Mode)
{
	switch (Mode)
	{
	case EDamageType::EDT_Normal:
		return GAMEPLAYTAG_Modes_Normal;

	case EDamageType::EDT_Infrared:
		return GAMEPLAYTAG_Modes_Infrared;

	case EDamageType::EDT_Ultraviolet:
		return GAMEPLAYTAG_Modes_Ultraviolet;

	case EDamageType::EDT_Gamma:
		return GAMEPLAYTAG_Modes_Gamma;

	default:
		// There should always be a corresponding type
		checkNoEntry();
		return FGameplayTag();
	}
}

FGameplayTag UGameplayUtilityBlueprintLibrary::GetAvailableModeAsGameplayTag(const EDamageType& Mode)
{
	switch (Mode)
	{
	case EDamageType::EDT_Normal:
		return GAMEPLAYTAG_Player_AvailableModes_Normal;

	case EDamageType::EDT_Infrared:
		return GAMEPLAYTAG_Player_AvailableModes_Infrared;

	case EDamageType::EDT_Ultraviolet:
		return GAMEPLAYTAG_Player_AvailableModes_Ultraviolet;

	case EDamageType::EDT_Gamma:
		return GAMEPLAYTAG_Player_AvailableModes_Gamma;

	default:
		// There should always be a corresponding type
		checkNoEntry();
		return FGameplayTag();
	}
}
