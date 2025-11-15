// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "LevelStreamingTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "DevCommons.h"

#include "AAPersistenceSubSystem.h"
#include "AASaveGameTypes.h"
#include "AASaveGame.h"

ALevelStreamingTrigger::ALevelStreamingTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ALevelStreamingTrigger::BeginPlay()
{
	Super::BeginPlay();

	// Bind to all colliders present that may have been added in Blueprint
	TInlineComponentArray<UPrimitiveComponent*> Primitives(this, false);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		Primitive->OnComponentBeginOverlap.AddDynamic(this, &ALevelStreamingTrigger::OnOverlapped);
	}

	
}

void ALevelStreamingTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelStreamingTrigger::OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

	check(PersistenceSubSystem);

	UAASaveGame* SaveGameObject = Cast<UAASaveGame>(PersistenceSubSystem->GetSaveGameObject());

	if (!SaveGameObject)
	{
		UE_LOG(VSLog, Warning, TEXT("SaveGame could not be cast. SaveGame might not be created yet?"))
	}

	// Loading all levels asyncronously
	for (ELevelName Level : LevelsToLoad)
	{
		const FName LevelAsName = UGameplayUtilityBlueprintLibrary::ConvertLevelToName(Level);
		SaveGameObject->LevelSaveObject.AddLevelToList(LevelAsName);

		ULevelStreaming* LevelStreamingObject = UGameplayStatics::GetStreamingLevel(this, LevelAsName);

		if (LevelStreamingObject)
		{
			if (!LevelStreamingObject->IsLevelLoaded())
			{
				FLatentActionInfo ActionInfo;
				UGameplayStatics::LoadStreamLevel(this, LevelAsName, true, false, ActionInfo);
			}
		}
		else
		{
			UE_LOG(VSLog, Error, TEXT("Level could not be loaded: %s"), *LevelAsName.ToString());
		}
	}

	// Unloading all levels asyncronously
	for (ELevelName Level : LevelsToUnload)
	{
		const FName LevelAsName = UGameplayUtilityBlueprintLibrary::ConvertLevelToName(Level);
		SaveGameObject->LevelSaveObject.RemoveLevelFromList(LevelAsName);

		ULevelStreaming* LevelStreamingObject = UGameplayStatics::GetStreamingLevel(this, LevelAsName);

		if (LevelStreamingObject)
		{
			if (LevelStreamingObject->IsLevelLoaded())
			{
				FLatentActionInfo ActionInfo;
				UGameplayStatics::UnloadStreamLevel(this, LevelAsName, ActionInfo, false);
			}
		}
		else
		{
			UE_LOG(VSLog, Error, TEXT("Level could not be loaded: %s"), *LevelAsName.ToString());
		}
	}
}

