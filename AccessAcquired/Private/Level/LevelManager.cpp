// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "LevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstanceInterface.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "DevCommons.h"

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

}

void ALevelManager::LoadLevelsBlocking(TArray<FName> LevelsToLoad)
{
}

