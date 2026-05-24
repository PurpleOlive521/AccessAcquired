// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "LevelStreamingTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "DevCommons.h"

ALevelStreamingTrigger::ALevelStreamingTrigger()
{
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

	// Loading all levels asyncronously
	for (ELevelName Level : LevelsToLoad)
	{
		const FName LevelAsName = UGameplayUtilityBlueprintLibrary::ConvertLevelToName(Level);

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

