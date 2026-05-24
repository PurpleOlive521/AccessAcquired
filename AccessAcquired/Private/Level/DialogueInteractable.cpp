// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "DialogueInteractable.h"
#include "DevCommons.h"

ADialogueInteractable::ADialogueInteractable()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADialogueInteractable::BeginPlay()
{
	Super::BeginPlay();

	TInlineComponentArray<USceneComponent*> SceneComponents(this, false);

	for (USceneComponent* SceneComp : SceneComponents)
	{
		if (SceneComp->GetName() == "WidgetLocation")
		{
			WidgetLocationComponent = SceneComp;
			break;
		}
	}

	if (!WidgetLocationComponent)
	{
		UE_LOG(VSLog, Error, TEXT("Could not find WidgetComponent named WidgetLocation!"))
	}
}

bool ADialogueInteractable::OnSerialize(FSaveGameArchive& Archive, bool bIsLoading)
{
	if (LOADING)
	{
		K2_SetInteractableState(bInteracted);
	}

	return true;
}

FInteractionParams ADialogueInteractable::GetInteractionData_Implementation()
{
	if (WidgetLocationComponent)
	{
		InteractionParams.InteractWidgetLocation = WidgetLocationComponent->GetComponentLocation();
	}

	return InteractionParams;
}

void ADialogueInteractable::SetInteractableState(bool bInInteracted)
{
	if (bInteracted == bInInteracted)
	{
		return;
	}

	bInteracted = bInInteracted;

	K2_SetInteractableState(bInteracted);
}
