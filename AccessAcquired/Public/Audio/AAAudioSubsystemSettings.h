// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include "AAAudioSubsystemSettings.generated.h"

/**
 * Audio Subsystem settings.
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Audio Subsystem Settings"))
class VERTICALSLICE_API UAudioSubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "AudioSubsystemSettings")
	UFMODEvent* GetRandomBattleMusic() const;

	UPROPERTY(EditAnywhere, Config, Category = "Music")
	TSoftObjectPtr<UFMODEvent> DefaultAmbience = nullptr;

	UPROPERTY(EditAnywhere, Config, Category = "Music")
	TSoftObjectPtr<UFMODEvent> StrongEnemyBattleMusic = nullptr;

	UPROPERTY(EditAnywhere, Config, Category = "Music")
	TArray<TSoftObjectPtr<UFMODEvent>> DefaultBattleMusic;
};