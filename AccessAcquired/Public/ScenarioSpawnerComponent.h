// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyBase.h"
#include "ScenarioVolume.h"
#include "SaveableObjectInterface.h"
#include "ScenarioSpawnerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveDefeatedSignature, UScenarioSpawnerComponent*, Spawner, int32, WaveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAllWavesDefeatedSignature, UScenarioSpawnerComponent*, Spawner);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VERTICALSLICE_API UScenarioSpawnerComponent : public UActorComponent, public ISaveableObjectInterface
{
	GENERATED_BODY()

public:
	UScenarioSpawnerComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// --- Begin SaveableObject Interface
	virtual bool OnSerialize(FSaveGameArchive& Archive, bool bIsLoading) override;
	// --- End SaveableObject Interface

	// Spawn a specific enemy group.
	UFUNCTION(BlueprintCallable, Category = "ScenarioSpawner")
	void SpawnEnemyGroup(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "ScenarioSpawner")
	void SpawnNextEnemyGroup();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScenarioSpawner")
	AScenarioVolume* GetVolume();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScenarioSpawner")
	[[nodiscard]] TArray<AEnemySpawnpoint*> GetSpawnpoints();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScenarioSpawner")
	void ShuffleSpawnpoints(TArray<AEnemySpawnpoint*>& OutShuffled) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ScenarioSpawner")
	bool HasWave(int32 Index);

	UFUNCTION()
	void OnSpawnedEnemyKilled(AEnemyBase* Enemy);

	UFUNCTION()
	void OnEnemySpawned(AEnemySpawnpoint* Spawnpoint, AEnemyBase* Enemy);

	void BindToSpawnpoint(AEnemySpawnpoint* Spawnpoint);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ScenarioSpawner")
	TArray<TObjectPtr<UScenario>> SpawnerWaves;

	// Spawns on receiving BeginPlay. 
	// Will be prevented by any Spawnpoint intending to spawn on Beginplay, in which case bSpawnOncePreviousGroupIsDead will be responsible for spawning enemies.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ScenarioSpawner")
	bool bSpawnOnBeginPlay = false;

	// Spawn the next group automatically once ALL prior enemies have been killed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ScenarioSpawner")
	bool bSpawnOncePreviousGroupIsDead = false;

	// Any spawned enemy will immediately be hostile and pursue the player.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ScenarioSpawner")
	bool bForceTargetPlayer = false;

	UPROPERTY(SaveGame)
	int32 CurrentGroupIndex = 0;

	UPROPERTY(SaveGame)
	TSet<TObjectPtr<AEnemyBase>> SpawnedEnemies;

	UPROPERTY(SaveGame)
	int32 EnemiesRemainingBeforeNextWave = 0;

	TWeakObjectPtr<AScenarioVolume> BoundVolume = nullptr;

public:

	// --- Delegates
	UPROPERTY(BlueprintAssignable, Category = "ScenarioSpawner")
	FOnWaveDefeatedSignature OnWaveDefeatedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "ScenarioSpawner")
	FOnAllWavesDefeatedSignature OnAllWavesDefeatedDelegate;
};
