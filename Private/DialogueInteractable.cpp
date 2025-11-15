// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "DialogueInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "DevCommons.h"

#include "AAPersistenceSubSystem.h"
#include "AASaveGameTypes.h"
#include "AASaveGame.h"

ADialogueInteractable::ADialogueInteractable()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADialogueInteractable::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

	check(PersistenceSubSystem);

	PersistenceSubSystem->RegisterAndLoadObject(this);

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

void ADialogueInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ADialogueInteractable::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::RemovedFromWorld)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
		UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

		check(PersistenceSubSystem);

		PersistenceSubSystem->RequestSaveObject(this);
	}
}

void ADialogueInteractable::PostEditImport()
{
	Id.Reset();
}

void ADialogueInteractable::SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	FDialogueInteractableSaveObject SaveObject;
	SaveObject.ValidityKey.MakeValid();
	SaveObject.bHasBeenInteractedWith = bInteractedWithPreviously;

	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);
	AASaveGameObject->SaveDialogueInteractable(Id, SaveObject);
}

void ADialogueInteractable::LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);

	const FDialogueInteractableSaveObject& SaveObject = AASaveGameObject->LoadDialogueInteractable(Id);

	if (!SaveObject.ValidityKey.IsValid())
	{
		return;
	}

	SetInteractableState(SaveObject.bHasBeenInteractedWith);
}

void ADialogueInteractable::Interact_Implementation(AActor* Interactor)
{

}

FInteractionParams ADialogueInteractable::GetInteractionData_Implementation()
{
	if (WidgetLocationComponent)
	{
		InteractionParams.InteractWidgetLocation = WidgetLocationComponent->GetComponentLocation();
	}
	return InteractionParams;
}

void ADialogueInteractable::SetInteractableState(bool bInInteractedWithPreviously)
{
	if (bInteractedWithPreviously == bInInteractedWithPreviously)
	{
		return;
	}

	bInteractedWithPreviously = bInInteractedWithPreviously;

	K2_SetInteractableState(bInteractedWithPreviously);
}
