// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "ScenarioTypes.h"
#include "EncounterTypes.h"
#include "Scenario.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UScenario : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UScenario() = default;

	// --- Begin UObject Interface
	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	// --- End UObject Interface
#endif //WITH_EDITOR

	// --- Begin UPrimaryDataAsset Interface
	FPrimaryAssetId GetPrimaryAssetId() const override;
	// --- End UPrimaryDataAsset Interface

	void BindToAssets();

	void UnbindFromAssets();

	// An Asset we depend on has been edited, moved or removed.
	void OnAssetInvalidated();

	UEncounter* GetCurrentEncounter() const;

protected:
	UPROPERTY(EditAnywhere)
	FScenarioEntry Entry;

public:

	// --- Delegates
	FOnAssetChainInvalidatedSignature OnAssetChainInvalidatedDelegate;
};
