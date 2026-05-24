// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "ScenarioSpawnerComponent.h"
#include "GameplayPersistenceSubsystem.h"
#include "ScenarioUserComponent.h"
#include "Scenario.h"
#include "Encounter.h"
#include "EnemySpawnpoint.h"
#include "EnemyAIControllerBase.h"
#include "Kismet/GameplayStatics.h"

UScenarioSpawnerComponent::UScenarioSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UScenarioSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();	

	bool bHasSpawnOnBeginPlaySpawnpoint = false;
	TArray<AEnemySpawnpoint*> Spawnpoints = GetSpawnpoints();
	for (const auto& Spawnpoint : Spawnpoints)
	{
		BindToSpawnpoint(Spawnpoint);

		if (Spawnpoint->WillSpawnEnemyOnBeginPlay())
		{
			bHasSpawnOnBeginPlaySpawnpoint = true;
		}
	}
	


	if (bSpawnOnBeginPlay && not bHasSpawnOnBeginPlaySpawnpoint)
	{
		if (UGameplayPersistenceSubsystem* GP = UGameplayPersistenceSubsystem::Get(this))
		{
			if (GP->IsGameLoadInProgress())
			{
				GP->OnGameLoadedDelegate.AddUniqueDynamic(this, &UScenarioSpawnerComponent::SpawnNextEnemyGroup);
				return;
			}

			SpawnNextEnemyGroup();
		}
	}
	
}

void UScenarioSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UScenarioSpawnerComponent::OnSerialize(FSaveGameArchive& Archive, bool bIsLoading)
{
	const bool bSuccess = Archive.SerializeField(TEXT("ScenarioSpawnerComponent"), [&](FStructuredArchive::FSlot Slot)
													{
														SerializeScriptProperties(Slot);
													});

	return bSuccess;
}

void UScenarioSpawnerComponent::SpawnEnemyGroup(int32 Index)
{
	if (not HasWave(Index))
	{
		return;
	}

	AScenarioVolume* Volume = GetVolume();
	if (not Volume)
	{
		return;
	}

	TObjectPtr<UScenario> CurrentWave = SpawnerWaves[Index];
	Volume->InjectScenario(CurrentWave);

	UEncounter* ActiveEncounter = CurrentWave->GetCurrentEncounter();
	if (not ActiveEncounter)
	{
		return;
	}

	TArray<AEnemySpawnpoint*> Spawnpoints = GetSpawnpoints();
	ShuffleSpawnpoints(Spawnpoints);

	const int32 SpawnpointCount = Spawnpoints.Num();
	const int32 SpawnCount = ActiveEncounter->SpawnerProperties.SpawnCount;

	for (int32 i = 0; i < SpawnCount; i++)
	{
		AEnemySpawnpoint* Spawnpoint = Spawnpoints[i % SpawnpointCount];
				
		Spawnpoint->SpawnEnemy();

		if (bForceTargetPlayer)
		{
			const int32 EnemyCount = Spawnpoint->GetSpawnedEnemyCount();

			if (AEnemyBase* Enemy = Spawnpoint->GetSpawnedEnemy(EnemyCount - 1))
			{
				if (AEnemyAIControllerBase* AIController = Cast<AEnemyAIControllerBase>(Enemy->GetAIController()))
				{
					UAIStateLayer_Attacking* HostileLayer = UAIStateLayerFactory::CreateLayer_Attacking(AIController, UGameplayStatics::GetPlayerCharacter(this, 0));
					AIController->PushState(HostileLayer);
				}
			}
		}
	}
}

void UScenarioSpawnerComponent::SpawnNextEnemyGroup()
{
	if (SpawnedEnemies.Num() == 0)
	{
		if (HasWave(CurrentGroupIndex))
		{
			SpawnEnemyGroup(CurrentGroupIndex++);
		}
	}
}

AScenarioVolume* UScenarioSpawnerComponent::GetVolume()
{
	if (not BoundVolume.IsValid())
	{
		AScenarioVolume* FoundVolume = Cast<AScenarioVolume>(GetOwner());
		BoundVolume = MakeWeakObjectPtr(FoundVolume);
	}

	return BoundVolume.Get();
}

TArray<AEnemySpawnpoint*> UScenarioSpawnerComponent::GetSpawnpoints()
{
	if (AScenarioVolume* Volume = GetVolume())
	{
		TArray<UScenarioUserComponent*> ScenarioUsers;
		Volume->GetUsers(ScenarioUsers);

		TArray<AEnemySpawnpoint*> Spawnpoints;

		for (const auto& User : ScenarioUsers)
		{
			if (AEnemySpawnpoint* Spawnpoint = Cast<AEnemySpawnpoint>(User->GetOwner()))
			{
				Spawnpoints.Add(Spawnpoint);
			}
		}

		return Spawnpoints;
	}

	return TArray<AEnemySpawnpoint*>();
}

void UScenarioSpawnerComponent::ShuffleSpawnpoints(TArray<AEnemySpawnpoint*>& OutShuffled) const
{
	int32 LastIndex = OutShuffled.Num() - 1;

	for (int32 i = 0; i <= LastIndex; ++i)
	{	
		int32 Index = FMath::RandRange(i, LastIndex);
		if (i != Index)
		{
			OutShuffled.Swap(i, Index);
		}
	}
}

bool UScenarioSpawnerComponent::HasWave(int32 Index)
{
	return Index >= 0 && Index < SpawnerWaves.Num();
}

void UScenarioSpawnerComponent::OnSpawnedEnemyKilled(AEnemyBase* Enemy)
{
	if (SpawnedEnemies.Contains(Enemy))
	{
		EnemiesRemainingBeforeNextWave--;

		if (bSpawnOncePreviousGroupIsDead && EnemiesRemainingBeforeNextWave <= 0)
		{
			if (EnemiesRemainingBeforeNextWave == 0)
			{
				OnWaveDefeatedDelegate.Broadcast(this, CurrentGroupIndex - 1);

				if (not HasWave(CurrentGroupIndex))
				{
					OnAllWavesDefeatedDelegate.Broadcast(this);
				}
			}

			SpawnNextEnemyGroup();
		}

		SpawnedEnemies.Remove(Enemy);
	}
}

void UScenarioSpawnerComponent::OnEnemySpawned(AEnemySpawnpoint* Spawnpoint, AEnemyBase* Enemy)
{
	if (not SpawnedEnemies.Contains(Enemy))
	{
		SpawnedEnemies.Add(Enemy);
		EnemiesRemainingBeforeNextWave++;
	}
}

void UScenarioSpawnerComponent::BindToSpawnpoint(AEnemySpawnpoint* Spawnpoint)
{
	Spawnpoint->OnSpawnedEnemyKilledDelegate.AddUniqueDynamic(this, &UScenarioSpawnerComponent::OnSpawnedEnemyKilled);
	Spawnpoint->OnEnemySpawnedDelegate.AddUniqueDynamic(this, &UScenarioSpawnerComponent::OnEnemySpawned);
}


