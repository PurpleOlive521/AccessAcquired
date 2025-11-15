#include "AASaveGameTypes.h"

#include "GameplayUtilityBlueprintLibrary.h"


FLevelSaveObject::FLevelSaveObject()
{
	LoadedLevels.Add(UGameplayUtilityBlueprintLibrary::ConvertLevelToName(ELevelName::ELN_Backstreets_StartArea));
}

void FLevelSaveObject::RemoveLevelFromList(FName LevelToRemove)
{
	LoadedLevels.Remove(LevelToRemove);
}

void FLevelSaveObject::AddLevelToList(FName LevelToLoad)
{
	LoadedLevels.AddUnique(LevelToLoad);
}
