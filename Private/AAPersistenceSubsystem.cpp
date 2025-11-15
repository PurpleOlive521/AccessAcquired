// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "AAPersistenceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "AASaveGameTypes.h"

#include "AASaveGame.h"


void UAAPersistenceSubsystem::CreateSaveGameObject()
{  
   UAASaveGame* AASaveGameObject = Cast<UAASaveGame>(UGameplayStatics::CreateSaveGameObject(UAASaveGame::StaticClass()));

   FLevelSaveObject DefaultLevelData;
   DefaultLevelData.LoadedLevels.Add(UGameplayUtilityBlueprintLibrary::ConvertLevelToName(DefaultLevel));

   AASaveGameObject->LevelSaveObject = DefaultLevelData;

   ActiveSaveGameObject = AASaveGameObject;
}
