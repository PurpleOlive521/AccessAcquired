// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "EncounterTypes.h"
#include "Encounter.generated.h"

constexpr int MAX_ALLOWED_DISTRIBUTION = 100;

struct FDistributionPrediction
{
	FDistributionPrediction() = default;

	explicit FDistributionPrediction(uint32 Overrides, uint32 Size);

	// The amount of entries required for the entire prediction.
	inline uint32 GetTotalCount() const;

	uint32 TotalOverrides = 0;

	uint32 FractionSize = 0;
};

/**
 *
 */
UCLASS()
class VERTICALSLICE_API UEncounter : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif //WITH_EDITOR
	// --- End UObject interface

	// --- Begin UPrimaryDataAsset Interface
	FPrimaryAssetId GetPrimaryAssetId() const override;
	// --- End UPrimaryDataAsset Interface

	void GetCurrentDistribution(TArray<int>& OutArray);

	FEncounterEntry GetEntryAtIndex(uint32 Index) const;

	// An Asset we depend on has been edited, moved or removed.
	void OnAssetInvalidated();

	UPROPERTY(EditAnywhere)
	FSpawnerProperties SpawnerProperties;

protected:

	void GenerateDistributions();

	// Returns the smallest amount of distributions required to accurately represent the ratios between entries.
	FDistributionPrediction GetRequiredDistributions() const;

	UPROPERTY(EditAnywhere)
	TArray<FEncounterEntry> Entries;

	// List of indices into Entries, generated based on the specified relative distribution between entries.
	TArray<int> EntryDistribution;

	// The amount of enemies required to fulfill the Ratios and Counts specified in Entries.
	UPROPERTY(VisibleDefaultsOnly)
	int MinimumEnemyCount = 0;

	bool bHasGeneratedDistribution = false;

public:

	// --- Delegates
	FOnAssetChainInvalidatedSignature OnAssetChainInvalidatedDelegate;
};