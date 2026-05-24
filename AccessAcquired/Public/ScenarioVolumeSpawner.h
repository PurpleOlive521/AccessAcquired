// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Level/ScenarioVolume.h"
#include "ScenarioSpawnerComponent.h"
#include "SaveableObjectInterface.h"
#include "ScenarioVolumeSpawner.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API AScenarioVolumeSpawner : public AScenarioVolume, public ISaveableObjectInterface
{
	GENERATED_BODY()
	
public:

	AScenarioVolumeSpawner(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	// --- Begin SaveableObject Interface
	virtual bool OnSerialize(FSaveGameArchive& Archive, bool bIsLoading) override;
	// --- End SaveableObject Interface

	UFUNCTION()
	void OnWaveDefeated(UScenarioSpawnerComponent* Spawner, int32 WaveIndex);

	UFUNCTION()
	void OnAllWavesDefeated(UScenarioSpawnerComponent* Spawner);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UScenarioSpawnerComponent> SpawnerComponent = nullptr;

public:
	// --- Delegates

	// These are shadowed from the ScenarioSpawnerComponent for convenience
	UPROPERTY(BlueprintAssignable, Category = "ScenarioVolumeSpawner")
	FOnWaveDefeatedSignature OnWaveDefeatedDelegate;

	// These are shadowed from the ScenarioSpawnerComponent for convenience
	UPROPERTY(BlueprintAssignable, Category = "ScenarioVolumeSpawner")
	FOnAllWavesDefeatedSignature OnAllWavesDefeatedDelegate;
};
