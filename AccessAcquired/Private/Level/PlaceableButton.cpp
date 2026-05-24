// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "PlaceableButton.h"
#include "Kismet/GameplayStatics.h"

APlaceableButton::APlaceableButton()
{
	PrimaryActorTick.bCanEverTick = false;
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

bool APlaceableButton::OnSerialize(FSaveGameArchive& Archive, bool bIsLoading)
{
	const bool bSuccess = Archive.SerializeField(TEXT("PlaceableButton"), [&](FStructuredArchive::FSlot Slot)
		{
			Slot << InteractionParams.bPreviouslyInteractedWith;
		});

	return bSuccess;
}
