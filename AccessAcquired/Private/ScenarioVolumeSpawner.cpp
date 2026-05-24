// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "ScenarioVolumeSpawner.h"

AScenarioVolumeSpawner::AScenarioVolumeSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpawnerComponent = ObjectInitializer.CreateDefaultSubobject<UScenarioSpawnerComponent>(this, TEXT("ScenarioSpawner"));
}

void AScenarioVolumeSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnerComponent->OnWaveDefeatedDelegate.AddUniqueDynamic(this, &AScenarioVolumeSpawner::OnWaveDefeated);
	SpawnerComponent->OnAllWavesDefeatedDelegate.AddUniqueDynamic(this, &AScenarioVolumeSpawner::OnAllWavesDefeated);
}

bool AScenarioVolumeSpawner::OnSerialize(FSaveGameArchive& Archive, bool bIsLoading)
{
	bool bSuccess = SpawnerComponent->OnSerialize(Archive, bIsLoading);

	return bSuccess;
}

void AScenarioVolumeSpawner::OnWaveDefeated(UScenarioSpawnerComponent* Spawner, int32 WaveIndex)
{
	OnWaveDefeatedDelegate.Broadcast(Spawner, WaveIndex);
}

void AScenarioVolumeSpawner::OnAllWavesDefeated(UScenarioSpawnerComponent* Spawner)
{
	OnAllWavesDefeatedDelegate.Broadcast(Spawner);
}
