// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "LockedDoor.h"
#include "Kismet/GameplayStatics.h"

#include "AAPersistenceSubSystem.h"
#include "AASaveGameTypes.h"
#include "AASaveGame.h"

ALockedDoor::ALockedDoor()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ALockedDoor::BeginPlay()
{
	Super::BeginPlay();

	DoorOpenAnimation = GetComponentByClass<UTimelineComponent>();
	check(DoorOpenAnimation);

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

	check(PersistenceSubSystem);

	PersistenceSubSystem->RegisterAndLoadObject(this);
}

void ALockedDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ALockedDoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::RemovedFromWorld)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
		UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

		check(PersistenceSubSystem);

		PersistenceSubSystem->RequestSaveObject(this);
	}
}

void ALockedDoor::PostEditImport()
{
	Id.Reset();
}

void ALockedDoor::SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	FLockableDoorSaveObject SaveObject;
	SaveObject.ValidityKey.MakeValid();
	SaveObject.bIsOpen = bIsOpen;

	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);
	AASaveGameObject->SaveLockableDoor(Id, SaveObject);
}

void ALockedDoor::LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);

	const FLockableDoorSaveObject& SaveObject = AASaveGameObject->LoadLockableDoor(Id);

	if (!SaveObject.ValidityKey.IsValid())
	{
		return;
	}

	MatchDoorAnimationToState(SaveObject.bIsOpen);
}

void ALockedDoor::OpenDoor()
{
	if (bIsAnimationPlaying || bIsOpen)
	{
		return;
	}

	bIsOpen = true;
	OnDoorStateChangedDelegate.Broadcast(true);

	PlayDoorAnimation(bIsOpen);
}

void ALockedDoor::CloseDoor()
{
	if (bIsAnimationPlaying || !bIsOpen)
	{
		return;
	}

	bIsOpen = false;
	OnDoorStateChangedDelegate.Broadcast(false);

	PlayDoorAnimation(bIsOpen);
}

void ALockedDoor::SetDoorState(bool bShouldOpen)
{
	if (bIsOpen == bShouldOpen)
	{
		return;
	}

	if (bShouldOpen)
	{
		OpenDoor();
	}
	else
	{
		CloseDoor();
	}
}

void ALockedDoor::InvertDoorState()
{
	if (bIsOpen)
	{
		CloseDoor();
	}
	else
	{
		OpenDoor();
	}
}

void ALockedDoor::PlayDoorAnimation(bool bShouldOpen)
{
	if (bShouldOpen)
	{
		DoorOpenAnimation->PlayFromStart();
	}
	else
	{
		DoorOpenAnimation->ReverseFromEnd();
	}
}

void ALockedDoor::AssignTimeline(UTimelineComponent* Timeline)
{
	DoorOpenAnimation = Timeline;
}

void ALockedDoor::MatchDoorAnimationToState(bool bShouldBeOpen)
{
	if (!DoorOpenAnimation)
	{
		return;
	}

	bIsAnimationPlaying = false;

	if (bShouldBeOpen)
	{
		DoorOpenAnimation->SetPlaybackPosition(DoorOpenAnimation->GetTimelineLength(), true, true);
		OpenDoor();
	}
	else
	{
		DoorOpenAnimation->SetPlaybackPosition(0, true, true);
		CloseDoor();
	}
}
