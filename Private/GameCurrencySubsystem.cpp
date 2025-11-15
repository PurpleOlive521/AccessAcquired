// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "GameCurrencySubsystem.h"

#include "GameplayPersistenceSubsystem.h"
#include "DevCommons.h"
#include "AASaveGameTypes.h"
#include "AASaveGame.h"

// Begin ISaveableObjectInterface

void UGameCurrencySubsystem::SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);

	AASaveGameObject->GameCurrencySaveObject.ValidityKey.MakeValid();
	AASaveGameObject->GameCurrencySaveObject.Intel = CurrentIntel;
}

void UGameCurrencySubsystem::LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);

	// Needs to be called first to avoid recursively calling itself from SetIntel
	bHasFetchedSaveData = true;

	CHECK_VALIDITY_EARLY_RETURN(AASaveGameObject->GameCurrencySaveObject);

	SetIntel(AASaveGameObject->GameCurrencySaveObject.Intel, false);
}

// End ISaveableObjectInterface


void UGameCurrencySubsystem::ChangeIntel(int Delta, bool bSendNotifications)
{
	CheckHasFetchedSave();

	CurrentIntel += Delta;

	// We dont allow debt & negatives to occur
	if (CurrentIntel < 0)
	{
		CurrentIntel = 0;
	}

	// Avoid any redundant broadcasts
	if (Delta != 0)
	{
		OnIntelChangedDelegate.Broadcast(CurrentIntel);

		if (bSendNotifications == true)
		{
			IntelNotificationsDelegate.Broadcast(Delta);
		}
	}
}

void UGameCurrencySubsystem::SetIntel(int NewValue, bool bSendNotifications)
{
	CheckHasFetchedSave();

	// We dont allow debt & negatives to occur
	if (NewValue < 0)
	{
		return;
	}

	const int OldIntel = CurrentIntel;

	CurrentIntel = NewValue;

	const int Delta = OldIntel - CurrentIntel;

	// Avoid any redundant broadcasts
	if (NewValue != OldIntel)
	{
		OnIntelChangedDelegate.Broadcast(CurrentIntel);

		if (bSendNotifications == true)
		{
			IntelNotificationsDelegate.Broadcast(Delta);
		}
	}
}

int UGameCurrencySubsystem::GetIntel()
{
	CheckHasFetchedSave();

	return CurrentIntel;
}

void UGameCurrencySubsystem::CheckHasFetchedSave()
{
	if (bHasFetchedSaveData == true)
	{
		return;
	}

	UGameplayPersistenceSubsystem* PersistenceSubsystem = GetGameInstance()->GetSubsystem<UGameplayPersistenceSubsystem>();

	if (PersistenceSubsystem == nullptr)
	{
		UE_LOG(VSLog, Error, TEXT("GameplayPersistanceSubsystem returned null. Intel out of sync with save data."))
		return;
	}

	PersistenceSubsystem->RegisterAndLoadObject(this);

	// We expect this to be switched since GameplayPersistance should have done its callback by now
	if (bHasFetchedSaveData == false)
	{
		UE_LOG(VSLog, Error, TEXT("Save data was not fetched correctly, flag not set."))
	}
}