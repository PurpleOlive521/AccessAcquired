// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "DestructibleBase.h"

#include "Kismet/GameplayStatics.h"
#include "ControllableCharacterInterface.h"

#include "AAPersistenceSubSystem.h"
#include "AASaveGameTypes.h"
#include "AASaveGame.h"

#include "Components/ShapeComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

ADestructibleBase::ADestructibleBase()
{
	// No need for tick currently
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ADestructibleBase::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

	check(PersistenceSubSystem);

	PersistenceSubSystem->RegisterAndLoadObject(this);
}

// Called every frame
void ADestructibleBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADestructibleBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::RemovedFromWorld)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
		UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

		check(PersistenceSubSystem);

		PersistenceSubSystem->RequestSaveObject(this);
	}
}

void ADestructibleBase::PostEditImport()
{
	Id.Reset();
}

void ADestructibleBase::OnDestructed(bool bIsOnLoad)
{
	/*
	
	// Get and disable all colliders
	TInlineComponentArray<UShapeComponent*> CollisionShapes(this, false);

	for (UShapeComponent* Collider : CollisionShapes)
	{
		Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// Get and disable all meshes, static or not,
	TInlineComponentArray<UMeshComponent*> MeshComponents(this, false);

	for (UMeshComponent* Mesh : MeshComponents)
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	

	// Get and disable all colliders present
	TInlineComponentArray<UPrimitiveComponent*> Primitives(this, false);

	for (UPrimitiveComponent* Primitive : Primitives)
	{
		Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	*/

	// Disable all collisions for the actor
	SetActorEnableCollision(false);

	// Make Actor invisible
	GetRootComponent()->SetVisibility(false, true);

	// Only inform & reward the player if not called as part of the loading process
	if (!bIsOnLoad)
	{
		ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);

		if (IControllableCharacterInterface* CharacterInterface = Cast<IControllableCharacterInterface>(Player))
		{
			FEnemyDeathReward Rewards(GainedExperience, GainedIntel);

			CharacterInterface->Execute_OnEntityKilled(Cast<UObject>(Player), this, Rewards);
		}
	}

	K2_OnDestructed(bIsOnLoad);
}

void ADestructibleBase::SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	FDestructibleSaveObject SaveObject;
	SaveObject.ValidityKey.MakeValid();
	SaveObject.bIsDestroyed = bIsDestroyed;

	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);
	AASaveGameObject->SaveDestructible(Id, SaveObject);
}

void ADestructibleBase::LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);

	const FDestructibleSaveObject& SaveObject = AASaveGameObject->LoadDestructible(Id);

	if (!SaveObject.ValidityKey.IsValid())
	{
		return;
	}

	bIsDestroyed = SaveObject.bIsDestroyed;

	if(bIsDestroyed)
	{
		OnDestructed(true);
	}
}

FAAResolvedDamageEvent ADestructibleBase::ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent)
{
	FAAResolvedDamageEvent ResolvedDamageEvent(DamageEvent);
	ResolvedDamageEvent.EventStates.bWasEventIgnored = false;

	Health -= DamageEvent.StaggerDamage;
	ResolvedDamageEvent.DamageTaken = DamageEvent.StaggerDamage;

	if (Health <= 0)
	{
		OnDestructed(false);
	}
	else
	{
		K2_OnDamageTaken();
	}

	return ResolvedDamageEvent;
}

bool ADestructibleBase::IsDead_Implementation()
{
	return bIsDestroyed;
}

ETeam ADestructibleBase::GetTeam_Implementation()
{
	return Team;
}

