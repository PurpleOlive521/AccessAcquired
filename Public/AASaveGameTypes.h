// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "GameplaySaveGameTypes.h"

#include "AASaveGameTypes.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSaveObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FStructValidityKey ValidityKey;

	UPROPERTY(BlueprintReadWrite)
	FTransform Transform = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite)
	FRotator ControlRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite)
	FGameplaySystemSaveObject GameplaySystemSaveObject;
};

USTRUCT(BlueprintType)
struct FGenericEnemySaveObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FStructValidityKey ValidityKey;

	UPROPERTY(BlueprintReadWrite)
	FTransform Transform = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite)
	FGameplaySystemSaveObject GameplaySystemSaveObject;

	UPROPERTY(BlueprintReadWrite)
	bool bIsDead = false;
};

USTRUCT(BlueprintType)
struct FPlaceableButtonSaveObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FStructValidityKey ValidityKey;

	UPROPERTY(BlueprintReadWrite)
	bool bPreviouslyPressed = false;
};

USTRUCT(BlueprintType)
struct FLockableDoorSaveObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FStructValidityKey ValidityKey;

	UPROPERTY(BlueprintReadWrite)
	bool bIsOpen = false;
};

USTRUCT(BlueprintType)
struct FDestructibleSaveObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FStructValidityKey ValidityKey;

	UPROPERTY(BlueprintReadWrite)
	bool bIsDestroyed = false;
};

USTRUCT(BlueprintType)
struct FLevelSaveObject
{
	GENERATED_BODY()

	FLevelSaveObject();

	// Ensures that the level is not already added.
	void AddLevelToList(FName LevelToAdd);

	void RemoveLevelFromList(FName LevelToRemove);

	UPROPERTY(BlueprintReadWrite)
	TArray<FName> LoadedLevels;
};

USTRUCT(BlueprintType)
struct FDialogueInteractableSaveObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FStructValidityKey ValidityKey;

	UPROPERTY(BlueprintReadWrite)
	bool bHasBeenInteractedWith = false;
};

USTRUCT(BlueprintType)
struct FDialogueTriggerSaveObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FStructValidityKey ValidityKey;

	UPROPERTY(BlueprintReadWrite)
	bool bHasBeenTriggered = false;
};

USTRUCT(BlueprintType)
struct FSkillTreeSaveObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, bool> UnlockedSkillTable;
};

USTRUCT(BlueprintType)
struct FGameCurrencySaveObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FStructValidityKey ValidityKey;

	UPROPERTY(BlueprintReadWrite)
	int Intel = 0;
};