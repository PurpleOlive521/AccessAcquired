// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SaveableObjectInterface.h"
#include "EnemySpawnpoint.generated.h"

class UScenarioUserComponent;
class USphereComponent;
class UEnemyDataAsset;
class AEnemyBase;
class ATraversablePath;

constexpr float NO_SPAWN_RADIUS = 0.0f;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnedEnemyKilledSignature, AEnemyBase*, Enemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemySpawnedSignature, AEnemySpawnpoint*, Spawnpoint, AEnemyBase*, Enemy);

UCLASS()
class VERTICALSLICE_API AEnemySpawnpoint : public AActor, public ISaveableObjectInterface
{
	GENERATED_BODY()
	
public:	
	AEnemySpawnpoint(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

#if WITH_EDITORONLY_DATA

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	void TrySpawnEnemyStandIn();

	void VisualizeParameters() const;

#endif // WITH_EDITORONLY_DATA

	void BindToAssets();

	void UnbindFromAssets();

	void BindToSpawnedEnemy(AEnemyBase* Enemy);

	UFUNCTION()
	void BindToSpawnedEnemies();

	// An Asset we depend on has been edited, moved or removed.
	void OnAssetInvalidated();

	UFUNCTION()
	void PerformSpawnOnBeginPlay();

	bool CanSpawnEnemyOnBeginPlay();

	bool WillSpawnEnemyOnBeginPlay() const;

	UFUNCTION(BlueprintCallable, Category = "EnemySpawnpoint")
	bool SpawnEnemy();

	// Overrides any current Enemy parameters and uses the EnemyToInject instead.
	UFUNCTION(BlueprintCallable, Category = "EnemySpawnpoint")
	bool SpawnEnemy_Injected(UEnemyDataAsset* EnemyToInject, int Level);

	// Will spawn the Enemy if this is called before the EnemySpawnpoints own BeginPlay
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EnemySpawnpoint")
	AEnemyBase* GetSpawnedEnemy(int32 Index = 0);

	int32 GetSpawnedEnemyCount() const;

	// Can return nullptr.
	UFUNCTION(BlueprintCallable)
	ATraversablePath* GetTraversablePath() const;

	UFUNCTION()
	void OnSpawnedEnemyKilled(AEnemyBase* Enemy);

protected:

	bool SpawnEnemy_Internal(UEnemyDataAsset* EnemyToSpawn, int Level);

	// Assigns any properties we want to pass onto spawned enemies.
	void AssignEnemyProperties(AEnemyBase* SpawnedActor, int Level);

	UPROPERTY(EditInstanceOnly, Category = "EnemySpawnpoint")
	bool bSpawnOnBeginPlay = true;

	UPROPERTY(EditInstanceOnly, Category = "EnemySpawnpoint")
	float SpawnRadius = NO_SPAWN_RADIUS;

	UPROPERTY(EditInstanceOnly, Category = "EnemySpawnpoint|Level")
	TObjectPtr<ATraversablePath> TraversablePath = nullptr;

	UPROPERTY(SaveGame)
	TArray<TWeakObjectPtr<AEnemyBase>> SpawnedEnemies;

	int ParticipantId = 0;

	// Used to ensure a unique name for spawned enemies, even in loaded games.
	UPROPERTY(SaveGame)
	int32 EnemySpawnedCount = 0;

	UPROPERTY(SaveGame)
	bool bSpawnedOnBeginPlay = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySpawnpoint")
	TObjectPtr<USceneComponent> CustomRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySpawnpoint")
	TObjectPtr<UScenarioUserComponent> ScenarioComponent = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySpawnpoint")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh = nullptr;

	TArray<UEnemyDataAsset*> OverrideEnemy;
#endif // WITH_EDITORONLY_DATA

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySpawnpoint")
	TObjectPtr<USphereComponent> Collider = nullptr;

public:
	// --- Delegates
	UPROPERTY(BlueprintAssignable)
	FOnSpawnedEnemyKilledSignature OnSpawnedEnemyKilledDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnEnemySpawnedSignature OnEnemySpawnedDelegate;
};
