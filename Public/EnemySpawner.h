// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "EnemyBase.h"
#include "GameplayEffect.h"

#include "EnemySpawner.generated.h"

class AEnemySpawnLocation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyGroupDefeated, int, GroupIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllEnemyGroupsDefeated);

USTRUCT(BlueprintType)
struct VERTICALSLICE_API FEnemyTypeContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<AEnemyBase> EnemyType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int EnemyLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AmountToSpawn = 1;
};

USTRUCT(BlueprintType)
struct VERTICALSLICE_API FEnemyGroup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GroupName = "New Enemy Group";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "EnemyType"))
	TArray<FEnemyTypeContainer> EnemyTypeContainer;

	UPROPERTY(BlueprintReadWrite)
	bool bHasSpawned = false;
};


UCLASS()
class VERTICALSLICE_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

private:

	// Any alive enemies that this spawner is responsible for wil prohibit automatically spawning the next group.
	bool bIsEnemiesAlive = false;

	// The current group to spawn.
	int CurrentGroupIndex = 0;

	// Flagged when in the process of spawning enemies.
	bool bIsBusy;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Will load all soft class pointers present in the LoadedEnemyMap, and replace the Value with a AActor pointer
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "EnemySpawner")
	void RequestLoadFromContainer();

	// Expected callback from RequestLoadFromContainer. Responsible for spawning all successfully loaded enemies in a group
	UFUNCTION(BlueprintCallable, Category = "EnemySpawner")
	void SpawnLoadedEnemies();

	// Allows blueprint to perform logic on the newly spawned enemies
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "EnemySpawner")
	void BP_OnEnemiesSpawned(const TArray<AActor*>& OutSpawnedEnemies, const FEnemyGroup& SpawnedGroup);

	FVector GetSpawnPoint();

	// Already placed Enemies can be added to this manually in the Editor, to make them count as a "first" group by default.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySpawner")
	TSet<AActor*> SpawnedEnemies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySpawner", meta = (TitleProperty = "GroupName"))
	TArray<FEnemyGroup> EnemyGroups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySpawner")
	TArray<TObjectPtr<AEnemySpawnLocation>> CustomSpawnLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySpawner")
	bool bUseCustomSpawnLocations = false;

	// The radius around the SpawnPoint that enemies can be spawned in.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySpawner")
	float SpawnRadius = 100.0f;

	// Spawn the first group immediately. Blocked if enemies are already attached to this spawner!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySpawner")
	bool bSpawnOnBeginPlay = false;

	// Spawn the next group automatically once ALL prior enemies have been killed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySpawner")
	bool bSpawnOncePreviousGroupIsDead = false;

	// Any spawned enemy will immediately be hostile and pursue the player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySpawner")
	bool bImmediatelyAgroOnPlayer = false;

	// The currently loaded enemies
	UPROPERTY(BlueprintReadWrite, Category = "EnemySpawner")
	TMap<TSoftClassPtr<AEnemyBase>, TSubclassOf<AActor>> LoadedEnemyMap;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Spawn a specific enemy group. Can fail if the spawner is already busy 
	UFUNCTION(BlueprintCallable, Category = "EnemySpawner")
	void SpawnEnemyGroup(int GroupToSpawn);

	// Gets the next group to spawn. Can fail if the spawner is already busy 
	UFUNCTION(BlueprintCallable, Category = "EnemySpawner")
	void SpawnNextEnemyGroup();


	// --- Delegates

	UPROPERTY(BlueprintAssignable)
	FOnEnemyGroupDefeated OnEnemyGroupDefeatedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnAllEnemyGroupsDefeated OnAllEnemyGroupsDefeatedDelegate;
};
