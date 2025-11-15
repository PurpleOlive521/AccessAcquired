// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "PlaceableButton.h"
#include "Kismet/GameplayStatics.h"

#include "AAPersistenceSubSystem.h"
#include "AASaveGameTypes.h"
#include "AASaveGame.h"

// Sets default values
APlaceableButton::APlaceableButton()
{
	// No need for tick currently
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APlaceableButton::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

	check(PersistenceSubSystem);

	PersistenceSubSystem->RegisterAndLoadObject(this);
}

// Called every frame
void APlaceableButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void APlaceableButton::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::RemovedFromWorld)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
		UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

		check(PersistenceSubSystem);

		PersistenceSubSystem->RequestSaveObject(this);
	}
}

void APlaceableButton::PostEditImport()
{
	Id.Reset();
}

void APlaceableButton::SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	FPlaceableButtonSaveObject SaveObject;
	SaveObject.ValidityKey.MakeValid();
	SaveObject.bPreviouslyPressed = InteractionParams.bPreviouslyInteractedWith;

	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);
	AASaveGameObject->SavePlaceableButton(Id, SaveObject);
}

void APlaceableButton::LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);

	const FPlaceableButtonSaveObject& SaveObject = AASaveGameObject->LoadPlaceableButton(Id);

	if (!SaveObject.ValidityKey.IsValid())
	{
		return;
	}

	InteractionParams.bPreviouslyInteractedWith = SaveObject.bPreviouslyPressed;


}

void APlaceableButton::Interact_Implementation(AActor* Interactor)
{
	InteractionParams.bPreviouslyInteractedWith = true;

	K2_OnButtonPressed();

	OnButtonPressedDelegate.Broadcast();
}

FInteractionParams APlaceableButton::GetInteractionData_Implementation()
{
	InteractionParams.InteractWidgetLocation = GetWidgetLocation();
	return InteractionParams;
}
