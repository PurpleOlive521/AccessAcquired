// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "EnemySpawner.h"

#include "GameplaySystemOwnerInterface.h"
#include "EnemySpawnLocation.h"

#include "DevCommons.h"

class IReceiveDamageEventInterface;

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	// Enemies already present
	if (SpawnedEnemies.Num() > 0)
	{
		bIsEnemiesAlive = true;

		for (const AActor* Enemy : SpawnedEnemies)
		{
			// Assign enemy bounds or similar concept to the enemy here
		}

		return;
	}
	
	if (bSpawnOnBeginPlay == true)
	{
		SpawnNextEnemyGroup();
	}
}

void AEnemySpawner::SpawnLoadedEnemies()
{
	TArray<AActor*> NewlySpawnedEnemies;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn all enemy types
	for (const FEnemyTypeContainer& EnemyTypeContainer : EnemyGroups[CurrentGroupIndex].EnemyTypeContainer)
	{
		// No loaded enemy associated with this type
		if (!LoadedEnemyMap.Find(EnemyTypeContainer.EnemyType))
		{
			UE_LOG(VSLog, Error, TEXT("No Enemy type found, could not spawn Enemy."));
			return;
		}

		// Spawn each type X times
		for (int i = 0; i < EnemyTypeContainer.AmountToSpawn; i++)
		{
			// Get spawn position
			FVector EnemyPosition = GetSpawnPoint();
			if (bUseCustomSpawnLocations == true)
			{
				if (CustomSpawnLocations.Num() > 0)
				{
					int RandomIndex = FMath::RandRange(0, CustomSpawnLocations.Num() - 1);
					EnemyPosition = CustomSpawnLocations[RandomIndex]->GetSpawnLocation();
				}
				else
				{
					UE_LOG(VSLog, Warning, TEXT("No custom spawn locations set, using default spawn logic."));
				}
			}
			
			FTransform EnemyTransform = { FQuat::Identity, EnemyPosition };

			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(LoadedEnemyMap.Find(EnemyTypeContainer.EnemyType)->Get(), EnemyTransform, SpawnParams);

			// Make sure we dont continue with an invalid actor
			if (SpawnedActor == nullptr)
			{
				UE_LOG(VSLog, Error, TEXT("Failed to spawn enemy of type %s"), *EnemyTypeContainer.EnemyType.ToString());
				continue;
			}

			// Get the component ourselves, since the enemy hasnt grabbed it itself yet
			UGameplaySystemComponent* GameplaySystem = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(SpawnedActor);
			
			if (GameplaySystem)
			{
				GameplaySystem->SetEntityLevel(EnemyTypeContainer.EnemyLevel, false);
			}

			NewlySpawnedEnemies.Add(SpawnedActor);
		}
	}

	// Perform logic on each newly spawned enemy
	for (AActor* SpawnedActor : NewlySpawnedEnemies)
	{
		SpawnedEnemies.Add(SpawnedActor);
	}

	BP_OnEnemiesSpawned(NewlySpawnedEnemies, EnemyGroups[CurrentGroupIndex]);

	EnemyGroups[CurrentGroupIndex].bHasSpawned = true;
	CurrentGroupIndex++;
	bIsBusy = false;
}

FVector AEnemySpawner::GetSpawnPoint()
{
	FVector SpawnPoint;
	FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0, SpawnRadius);
	RandomOffset.Z = 0;
	SpawnPoint = GetActorLocation() + RandomOffset;

	return SpawnPoint;
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsBusy == true || bIsEnemiesAlive == false)
	{
		return;
	}

	bool bEnemiesAreStillAlive = false;

	TArray<AActor*> EnemiesToRemove;

	// Check if any enemy is still alive
	for (AActor* Enemy : SpawnedEnemies)
	{
		if (IsValid(Enemy) == true)
		{
			if (IReceiveDamageEventInterface::Execute_IsDead(Enemy) == false)
			{
				bEnemiesAreStillAlive = true;
				break;
			}
		}
	}
	
	if (bEnemiesAreStillAlive == false)
	{
		// First remove already dead or gc'd enemies
		SpawnedEnemies.Empty();

		bIsEnemiesAlive = false;
		OnEnemyGroupDefeatedDelegate.Broadcast(CurrentGroupIndex - 1);

		bool bHasAllGroupsSpawned = true;
		for (FEnemyGroup Group : EnemyGroups)
		{
			if (Group.bHasSpawned == false)
			{
				bHasAllGroupsSpawned = false;
			}
		}

		if (bHasAllGroupsSpawned == true)
		{
			OnAllEnemyGroupsDefeatedDelegate.Broadcast();
		}

		if (bSpawnOncePreviousGroupIsDead == true)
		{
			SpawnNextEnemyGroup();
		}
	}


}

void AEnemySpawner::SpawnEnemyGroup(int GroupToSpawn)
{
	if (bIsBusy == true)
	{
		return;
	}

	if (GroupToSpawn >= 0 && GroupToSpawn < EnemyGroups.Num())
	{
		if (EnemyGroups[GroupToSpawn].bHasSpawned == true)
		{
			UE_LOG(VSLog, Warning, TEXT("Attempted to spawn already spawned enemy group"));
			return;
		}

		bIsBusy = true;
		bIsEnemiesAlive = true;
		
		// Prep the container that the classes will be loaded into
		for (FEnemyTypeContainer EnemyTypeContainer : EnemyGroups[GroupToSpawn].EnemyTypeContainer)
		{
			LoadedEnemyMap.Add(EnemyTypeContainer.EnemyType, nullptr);
		}

		// Load enemies
		RequestLoadFromContainer();
	}
}

void AEnemySpawner::SpawnNextEnemyGroup()
{
	SpawnEnemyGroup(CurrentGroupIndex);
}


