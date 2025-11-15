#include "AASaveGame.h"

void UAASaveGame::SaveGenericEnemy(const FGuidTag& Id, const FGenericEnemySaveObject& SaveObject)
{
	EnemySaveToIdMap.Add(Id, SaveObject);
}

FGenericEnemySaveObject UAASaveGame::LoadGenericEnemy(const FGuidTag& Id)
{
	if (auto* ValuePtr = EnemySaveToIdMap.Find(Id))
	{
		return *ValuePtr;
	}

	return FGenericEnemySaveObject{};
}

void UAASaveGame::SavePlaceableButton(const FGuidTag& Id, const FPlaceableButtonSaveObject SaveObject)
{
	ButtonSaveToIdMap.Add(Id, SaveObject);
}

FPlaceableButtonSaveObject UAASaveGame::LoadPlaceableButton(const FGuidTag& Id)
{
	if (auto* ValuePtr = ButtonSaveToIdMap.Find(Id))
	{
		return *ValuePtr;
	}

	return FPlaceableButtonSaveObject{};
}

void UAASaveGame::SaveLockableDoor(const FGuidTag& Id, const FLockableDoorSaveObject SaveObject)
{
	LockableDoorSaveToIdMap.Add(Id, SaveObject);
}

FLockableDoorSaveObject UAASaveGame::LoadLockableDoor(const FGuidTag& Id)
{
	if (auto* ValuePtr = LockableDoorSaveToIdMap.Find(Id))
	{
		return *ValuePtr;
	}

	return FLockableDoorSaveObject{};
}

void UAASaveGame::SaveDestructible(const FGuidTag& Id, const FDestructibleSaveObject SaveObject)
{
	DestructibleSaveToIdMap.Add(Id, SaveObject);
}

FDestructibleSaveObject UAASaveGame::LoadDestructible(const FGuidTag& Id)
{
	if (auto* ValuePtr = DestructibleSaveToIdMap.Find(Id))
	{
		return *ValuePtr;
	}

	return FDestructibleSaveObject{};
}

void UAASaveGame::SaveDialogueTrigger(const FGuidTag& Id, const FDialogueTriggerSaveObject SaveObject)
{
	DialogueTriggerSaveToIdMap.Add(Id, SaveObject);
}

FDialogueTriggerSaveObject UAASaveGame::LoadDialogueTrigger(const FGuidTag& Id)
{
	if (auto* ValuePtr = DialogueTriggerSaveToIdMap.Find(Id))
	{
		return *ValuePtr;
	}

	return FDialogueTriggerSaveObject{};
}

void UAASaveGame::SaveDialogueInteractable(const FGuidTag& Id, const FDialogueInteractableSaveObject SaveObject)
{
	DialogueInteractableSaveToIdMap.Add(Id, SaveObject);
}

FDialogueInteractableSaveObject UAASaveGame::LoadDialogueInteractable(const FGuidTag& Id)
{
	if (auto* ValuePtr = DialogueInteractableSaveToIdMap.Find(Id))
	{
		return *ValuePtr;
	}

	return FDialogueInteractableSaveObject{};
}