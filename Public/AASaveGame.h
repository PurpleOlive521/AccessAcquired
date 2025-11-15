// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "GameplaySaveGame.h"
#include "AASaveGameTypes.h"

#include "AASaveGame.generated.h"

// Access Acquired-specific extension of the GameplaySaveGame class.
UCLASS()
class VERTICALSLICE_API UAASaveGame : public UGameplaySaveGame
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	void SaveGenericEnemy(const FGuidTag& Id, const FGenericEnemySaveObject& SaveObject);

	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	FGenericEnemySaveObject LoadGenericEnemy(const FGuidTag& Id);
	

	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	void SavePlaceableButton(const FGuidTag& Id, const FPlaceableButtonSaveObject SaveObject);
	
	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	FPlaceableButtonSaveObject LoadPlaceableButton(const FGuidTag& Id);


	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	void SaveLockableDoor(const FGuidTag& Id, const FLockableDoorSaveObject SaveObject);

	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	FLockableDoorSaveObject LoadLockableDoor(const FGuidTag& Id);


	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	void SaveDestructible(const FGuidTag& Id, const FDestructibleSaveObject SaveObject);

	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	FDestructibleSaveObject LoadDestructible(const FGuidTag& Id);


	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	void SaveDialogueTrigger(const FGuidTag& Id, const FDialogueTriggerSaveObject SaveObject);

	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	FDialogueTriggerSaveObject LoadDialogueTrigger(const FGuidTag& Id);

	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	void SaveDialogueInteractable(const FGuidTag& Id, const FDialogueInteractableSaveObject SaveObject);

	UFUNCTION(BlueprintCallable, Category = "AASaveGame")
	FDialogueInteractableSaveObject LoadDialogueInteractable(const FGuidTag& Id);


	// Save contents:

	UPROPERTY(BlueprintReadWrite, Category = "AASaveGame")
	FLevelSaveObject LevelSaveObject;

	UPROPERTY(BlueprintReadWrite, Category = "AASaveGame")
	FPlayerSaveObject PlayerSaveObject;

	UPROPERTY(BlueprintReadWrite, Category = "AASaveGame")
	FSkillTreeSaveObject SkillTreeSaveObject;

	UPROPERTY(BlueprintReadWrite, Category = "AASaveGame")
	FGameCurrencySaveObject GameCurrencySaveObject;

	// Lookup Tables:

	UPROPERTY(BlueprintReadWrite, Category = "AASaveGame")
	TMap<FGuidTag, FGenericEnemySaveObject> EnemySaveToIdMap;

	UPROPERTY(BlueprintReadWrite, Category = "AASaveGame")
	TMap<FGuidTag, FPlaceableButtonSaveObject> ButtonSaveToIdMap;

	UPROPERTY(BlueprintReadWrite, Category = "AASaveGame")
	TMap<FGuidTag, FLockableDoorSaveObject> LockableDoorSaveToIdMap;

	UPROPERTY(BlueprintReadWrite, Category = "AASaveGame")
	TMap<FGuidTag, FDestructibleSaveObject> DestructibleSaveToIdMap;

	UPROPERTY(BlueprintReadWrite, Category = "AASaveGame")
	TMap<FGuidTag, FDialogueTriggerSaveObject> DialogueTriggerSaveToIdMap;
	
	UPROPERTY(BlueprintReadWrite, Category = "AASaveGame")
	TMap<FGuidTag, FDialogueInteractableSaveObject> DialogueInteractableSaveToIdMap;
};
