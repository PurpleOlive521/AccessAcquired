// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "EnemySpawnpoint.h"
#include "ScenarioUserComponent.h"
#include "Encounter.h"
#include "ScenarioVolume.h"
#include "EnemyAsset.h"
#include "EnemyBase.h"
#include "Components/SphereComponent.h"
#include "SingleAnimationPlayData.h"
#include "DrawDebugHelpers.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameplaySystemComponent.h"
#include "GameplayPersistenceSubsystem.h"

AEnemySpawnpoint::AEnemySpawnpoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorHiddenInGame(true);

	CustomRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Custom Root"));
	CustomRoot->SetMobility(EComponentMobility::Movable);
	SetRootComponent(CustomRoot);

	Collider = ObjectInitializer.CreateDefaultSubobject <USphereComponent>(this, TEXT("Volume Collider"));
	Collider->SetMobility(EComponentMobility::Movable);
	Collider->SetupAttachment(CustomRoot);

	ScenarioComponent = ObjectInitializer.CreateDefaultSubobject<UScenarioUserComponent>(this, TEXT("Scenario"));

#if WITH_EDITORONLY_DATA
	SkeletalMesh = ObjectInitializer.CreateEditorOnlyDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Stand-in Mesh"));
	if (SkeletalMesh)
	{
		SkeletalMesh->SetupAttachment(CustomRoot);
		SkeletalMesh->SetMobility(EComponentMobility::Movable);
		SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	}
#endif // WITH_EDITORONLY_DATA
}

void AEnemySpawnpoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	// No bindings for temporary copies (created when dragging Actors in-editor, copying and similar actions)
	if (IsTemplate(RF_Transient))
	{
		return;
	}
#endif //WITH_EDITOR

	UnbindFromAssets();
	BindToAssets();

#if WITH_EDITORONLY_DATA
	TrySpawnEnemyStandIn();
#endif //WITH_EDITORONLY_DATA
}

void AEnemySpawnpoint::BeginPlay()
{
	Super::BeginPlay();

	if (UGameplayPersistenceSubsystem* GP = UGameplayPersistenceSubsystem::Get(this))
	{
		if (GP->IsGameLoadInProgress())
		{
			GP->OnGameLoadedDelegate.AddUniqueDynamic(this, &AEnemySpawnpoint::BindToSpawnedEnemies);
		}
	}

	if (bSpawnOnBeginPlay && not bSpawnedOnBeginPlay)
	{
		if (not CanSpawnEnemyOnBeginPlay())
		{
			if (UGameplayPersistenceSubsystem* GP = UGameplayPersistenceSubsystem::Get(this))
			{
				GP->OnGameLoadedDelegate.AddUniqueDynamic(this, &AEnemySpawnpoint::PerformSpawnOnBeginPlay);
				return;
			}
		}

		PerformSpawnOnBeginPlay();
	}	
}

void AEnemySpawnpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#if WITH_EDITORONLY_DATA

void AEnemySpawnpoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	VisualizeParameters();
}

void AEnemySpawnpoint::TrySpawnEnemyStandIn()
{
	SkeletalMesh->SetSkeletalMesh(nullptr);

	AScenarioVolume* ScenarioVolume = ScenarioComponent->GetVolume();
	if (!ScenarioVolume)
	{
		return;
	}

	FEncounterEntry Entry = ScenarioVolume->GetEnemyEntryById(ScenarioComponent->GetUserId());
	if (UEnemyDataAsset* Asset = Entry.GetEnemyAsset())
	{
		USkeletalMesh* Mesh = Asset->StandInMesh;
		SkeletalMesh->SetSkeletalMesh(Mesh);
		
		for (int32 i = 0; i < Asset->MeshMaterials.Num(); i++)
		{
			SkeletalMesh->SetMaterial(i, Asset->MeshMaterials[i]);
		}

		// Force the SkeletalMesh to update it's pose to match the animation
		SkeletalMesh->AnimationData.AnimToPlay = Asset->Pose;
		SkeletalMesh->AnimationData.bSavedLooping = false;
		SkeletalMesh->AnimationData.bSavedPlaying = false;
		SkeletalMesh->TickAnimation(0.f, false);
		SkeletalMesh->RefreshBoneTransforms();
	}
}

void AEnemySpawnpoint::VisualizeParameters() const
{
	const FVector VerticalOffset = { 0.0f, 0.0f, 10.0f };
	const FVector Start = GetActorLocation();
	const FVector End = Start + VerticalOffset;
	const int Segments = 12;
	const float Lifetime = 3.0f;

	DrawDebugCylinder(GetWorld(), Start, End, SpawnRadius, Segments, FColor::Red, false, Lifetime);
}

#endif // WITH_EDITORONLY_DATA

void AEnemySpawnpoint::BindToAssets()
{
	if (ScenarioComponent)
	{
		ScenarioComponent->OnAssetChainInvalidatedDelegate.AddUObject(this, &AEnemySpawnpoint::OnAssetInvalidated);
	}
}

void AEnemySpawnpoint::UnbindFromAssets()
{
	if (ScenarioComponent)
	{
		ScenarioComponent->OnAssetChainInvalidatedDelegate.RemoveAll(this);
	}
}

void AEnemySpawnpoint::BindToSpawnedEnemy(AEnemyBase* Enemy)
{
	Enemy->OnEnemyDeadDelegate.AddUniqueDynamic(this, &AEnemySpawnpoint::OnSpawnedEnemyKilled);
}

void AEnemySpawnpoint::BindToSpawnedEnemies()
{
	for (auto SpawnedEnemy : SpawnedEnemies)
	{
		if (AEnemyBase* Enemy = SpawnedEnemy.Get())
		{
			BindToSpawnedEnemy(Enemy);
		}
	}
}

void AEnemySpawnpoint::OnAssetInvalidated()
{
#if WITH_EDITORONLY_DATA
	TrySpawnEnemyStandIn();
#endif // WITH_EDITORONLY_DATA
}

void AEnemySpawnpoint::PerformSpawnOnBeginPlay()
{
	if (!bSpawnedOnBeginPlay)
	{
		bSpawnedOnBeginPlay = SpawnEnemy();
	}
}

bool AEnemySpawnpoint::CanSpawnEnemyOnBeginPlay()
{
	if (bSpawnOnBeginPlay)
	{
		if (not bSpawnedOnBeginPlay)
		{
			if (UGameplayPersistenceSubsystem* GP = UGameplayPersistenceSubsystem::Get(this))
			{
				// If we are awaiting a load, then we need to wait before trying to spawn our Enemy
				if (GP->IsGameLoadInProgress())
				{
					return false;
				}
			}
		}

		return true;
	}
	
	return false;
}

bool AEnemySpawnpoint::WillSpawnEnemyOnBeginPlay() const
{
	return bSpawnOnBeginPlay && bSpawnedOnBeginPlay;
}

bool AEnemySpawnpoint::SpawnEnemy()
{
	AScenarioVolume* ScenarioVolume = ScenarioComponent->GetVolume();
	if (!ScenarioVolume)
	{
		return false;
	}

	FEncounterEntry Entry = ScenarioVolume->GetEnemyEntryById(ScenarioComponent->GetUserId());
	if (UEnemyDataAsset* Asset = Entry.GetEnemyAsset())
	{
		return SpawnEnemy_Internal(Asset, Entry.Level);
	}

	return false;
}

bool AEnemySpawnpoint::SpawnEnemy_Injected(UEnemyDataAsset* EnemyToInject, int Level)
{
	return SpawnEnemy_Internal(EnemyToInject, Level);
}

AEnemyBase* AEnemySpawnpoint::GetSpawnedEnemy(int32 Index)
{
	// In case this is called ahead of us receiving the BeginPlay
	if (CanSpawnEnemyOnBeginPlay())
	{
		PerformSpawnOnBeginPlay();
	}

	if (Index >= 0 && Index < SpawnedEnemies.Num())
	{
		return SpawnedEnemies[Index].Get();
	}

	return nullptr;
}

int32 AEnemySpawnpoint::GetSpawnedEnemyCount() const
{
	return SpawnedEnemies.Num();
}

ATraversablePath* AEnemySpawnpoint::GetTraversablePath() const
{
	return TraversablePath;
}

void AEnemySpawnpoint::OnSpawnedEnemyKilled(AEnemyBase* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	for (int32 i = SpawnedEnemies.Num() - 1; i >= 0; i--)
	{
		if (SpawnedEnemies[i] == Enemy)
		{
			OnSpawnedEnemyKilledDelegate.Broadcast(Enemy);
			SpawnedEnemies.RemoveAt(i);
			continue;
		}

		if (!SpawnedEnemies[i].IsValid())
		{
			SpawnedEnemies.RemoveAt(i);
			continue;
		}
	}
}

bool AEnemySpawnpoint::SpawnEnemy_Internal(UEnemyDataAsset* EnemyToSpawn, int Level)
{
	if (!EnemyToSpawn)
	{
		return false;
	}

	FVector RandomOffset = FMath::VRand() * FMath::FRandRange(0, SpawnRadius);
	RandomOffset.Z = 70;
	const FVector SpawnLocation = GetActorLocation() + RandomOffset;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Format in the style "SpawnpointName_ClassName_UniqueNumber"
	FString Name = GetName() + TEXT("_") + EnemyToSpawn->EnemyClass->GetName() + FString::Printf(TEXT("_%d"), EnemySpawnedCount);
	Params.Name = FName(Name);
	Params.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	Params.OverrideLevel = GetLevel();

	EnemySpawnedCount++;

	AEnemyBase* SpawnedActor = GetWorld()->SpawnActor<AEnemyBase>(EnemyToSpawn->EnemyClass, SpawnLocation, GetActorRotation(), Params);
	check(SpawnedActor);

	if (not SpawnedActor)
	{
		return false;
	}

	SpawnedEnemies.Add(MakeWeakObjectPtr(SpawnedActor));

	AssignEnemyProperties(SpawnedActor, Level);

	OnEnemySpawnedDelegate.Broadcast(this, SpawnedActor);

	return true;
}

void AEnemySpawnpoint::AssignEnemyProperties(AEnemyBase* SpawnedActor, int Level)
{
	check(SpawnedActor);

	if (IGameplaySystemOwnerInterface* Interface = Cast<IGameplaySystemOwnerInterface>(SpawnedActor))
	{
		UGameplaySystemComponent* GameplaySystem = Interface->Execute_GetGameplaySystemComponent(SpawnedActor);

		check(GameplaySystem);

		GameplaySystem->SetEntityLevel(Level);
	}

	SpawnedActor->SetSpawnpoint(this);

	BindToSpawnedEnemy(SpawnedActor);
}

