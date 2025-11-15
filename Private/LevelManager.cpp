// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "LevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstanceInterface.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "DevCommons.h"

#include "AAPersistenceSubSystem.h"
#include "AASaveGameTypes.h"
#include "AASaveGame.h"


ALevelManager::ALevelManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ALevelManager::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);

	check(GameInstance);

	IGameInstanceInterface::Execute_OnLevelBeginPlay(GameInstance);
	
	// Start loading levels
	LoadLevelState();

	UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

	check(PersistenceSubSystem);

	// Start allowing objects to save to the (hopefully) synced SaveGame
	PersistenceSubSystem->SetSavingState(true);
}

void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelManager::UpdateExternalLevelState(TArray<ELevelName> InExternalLevels, bool InLoadExternalLevels)
{
	bLoadExternalLevels = InLoadExternalLevels;

	ExternalLevels = InExternalLevels;
}

void ALevelManager::ResetExternalLevelState()
{
	bLoadExternalLevels = false;
	
	ExternalLevels.Empty();
}

void ALevelManager::LoadLevelState()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

	check(PersistenceSubSystem);

	UAASaveGame* SaveGameObject = Cast<UAASaveGame>(PersistenceSubSystem->GetSaveGameObject());

	if (!SaveGameObject)
	{
		UE_LOG(VSLog, Warning, TEXT("SaveGame could not be cast. SaveGame might not be created yet?"));
		return;
	}

	TArray<FName> LevelsToLoad = SaveGameObject->LevelSaveObject.LoadedLevels;

	// Add any levels defined externally
	if (bLoadExternalLevels)
	{
		for (ELevelName LevelName : ExternalLevels)
		{
			LevelsToLoad.AddUnique(UGameplayUtilityBlueprintLibrary::ConvertLevelToName(LevelName));
		}
	}

	// Commence loading
	LoadLevelsBlocking(LevelsToLoad);
}

void ALevelManager::LoadLevelsBlocking(TArray<FName> LevelsToLoad)
{
	// Nothing left to load
	if (LevelsToLoad.Num() == 0)
	{
		return;
	}

	const FName LevelToLoad = LevelsToLoad.Pop();
	ULevelStreaming* LevelStreamingObject = UGameplayStatics::GetStreamingLevel(this, LevelToLoad);

	if (LevelStreamingObject)
	{
		if (!LevelStreamingObject->IsLevelLoaded())
		{
			FLatentActionInfo ActionInfo;
			UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, ActionInfo);

			// Continue calling until all levels are loaded & removed
			LoadLevelsBlocking(LevelsToLoad);
		}
		else
		{
			UE_LOG(VSLog, Log, TEXT("Requested level is already loaded: %s"), *LevelStreamingObject->GetWorldAssetPackageName());
		}
	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("Level could not be loaded: %s"), *LevelToLoad.ToString());
	}
}

