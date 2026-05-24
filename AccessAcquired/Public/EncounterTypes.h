// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Fraction.h"
#include "EncounterTypes.generated.h"

class UEnemyDataAsset;

constexpr int NO_OVERRIDE = 0;

// Used to trickle down invalidation calls to any underlying object in the Scenario system (UEncounter to UScenario to AScenarioVolume to UScenarioUserComponent, etc.)
DECLARE_MULTICAST_DELEGATE(FOnAssetChainInvalidatedSignature);

// Only used when a Spawner is responsible for spawning the Encounter
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FSpawnerProperties
{
	GENERATED_BODY()

	FSpawnerProperties() = default;

	UPROPERTY(EditAnywhere, Category = "SpawnerParams")
	int32 SpawnCount = 0;
};

USTRUCT(BlueprintType)
struct VERTICALSLICE_API FEncounterEntry
{
	GENERATED_BODY()

	FEncounterEntry() = default;

	// Loads the Asset. Returns nullptr if no asset exists or loading fails.
	UEnemyDataAsset* GetEnemyAsset() const;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UEnemyDataAsset> EnemyAsset = nullptr;

	// The presence of this enemy relative to other Entries. Only distributed amongst non-overriden entries.
	UPROPERTY(EditAnywhere, meta = (EditCondition = "OverrideCount == 0"))
	FFraction Fraction;

	// If entered, will ignore Ratio and use this count instead.
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	int OverrideCount = NO_OVERRIDE;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	int Level = 1;
};