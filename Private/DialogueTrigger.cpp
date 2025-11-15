// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "DialogueTrigger.h"
#include "Kismet/GameplayStatics.h"

#include "AAPersistenceSubSystem.h"
#include "AASaveGameTypes.h"
#include "AASaveGame.h"

ADialogueTrigger::ADialogueTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ADialogueTrigger::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

	check(PersistenceSubSystem);

	PersistenceSubSystem->RegisterAndLoadObject(this);
	
}

void ADialogueTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADialogueTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::RemovedFromWorld)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
		UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

		check(PersistenceSubSystem);

		PersistenceSubSystem->RequestSaveObject(this);
	}
}

void ADialogueTrigger::PostEditImport()
{
	Id.Reset();
}

void ADialogueTrigger::SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	FDialogueTriggerSaveObject SaveObject;
	SaveObject.ValidityKey.MakeValid();
	SaveObject.bHasBeenTriggered = bPreviouslyTriggered;
	
	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);
	AASaveGameObject->SaveDialogueTrigger(Id, SaveObject);
}

void ADialogueTrigger::LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);

	const FDialogueInteractableSaveObject& SaveObject = AASaveGameObject->LoadDialogueInteractable(Id);

	if (!SaveObject.ValidityKey.IsValid())
	{
		return;
	}
	
	bPreviouslyTriggered = SaveObject.bHasBeenInteractedWith;

}
