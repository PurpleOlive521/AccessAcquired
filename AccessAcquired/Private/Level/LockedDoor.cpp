// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "LockedDoor.h"
#include "GateComponent.h"
#include "GateControllerComponent.h"

ALockedDoor::ALockedDoor(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CustomRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Custom Root"));
	CustomRoot->SetMobility(EComponentMobility::Movable);
	SetRootComponent(CustomRoot);

	GateLeft = ObjectInitializer.CreateDefaultSubobject<UGateComponent>(this, TEXT("Gate Left"));
	GateLeft->SetMobility(EComponentMobility::Movable);
	GateLeft->SetupAttachment(CustomRoot);

	GateRight = ObjectInitializer.CreateDefaultSubobject<UGateComponent>(this, TEXT("Gate Right"));
	GateRight->SetMobility(EComponentMobility::Movable);
	GateRight->SetupAttachment(CustomRoot);

	GateController = ObjectInitializer.CreateDefaultSubobject<UGateControllerComponent>(this, TEXT("Gate Controller"));
}

void ALockedDoor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	GateController->SetGateStates_Instantaneous(StartingState);
}

void ALockedDoor::BeginPlay()
{
	Super::BeginPlay();

	GateController->SetGateStates_Instantaneous(StartingState);
}

bool ALockedDoor::OnSerialize(FSaveGameArchive& Archive, bool bIsLoading)
{
	return GateController->OnSerialize(Archive, bIsLoading);
}

UGateControllerComponent* ALockedDoor::GetGateController() const
{
	return GateController;
}
