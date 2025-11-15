// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "EnemySpawnLocation.h"

// Sets default values
AEnemySpawnLocation::AEnemySpawnLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawnLocation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemySpawnLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AEnemySpawnLocation::GetSpawnLocation()
{
	FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0, SpawnRadius);

	// This offset helps make sure it doesnt place the actor inside whatever object this might be ontop of.
	RandomOffset.Z = 70;
	FVector EnemyPosition = GetActorLocation() + RandomOffset;
	return EnemyPosition;
}

