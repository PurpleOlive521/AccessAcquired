// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameCurrencySubsystem.h"

#include "GameplayPersistenceSubsystem.h"
#include "DevCommons.h"
#include "GameplayTagMessageDefines.h"
#include "Kismet/GameplayStatics.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameFramework/Character.h"
#include "GameplaySystemComponent.h"

bool UGameCurrencySubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const 
{
	if (WorldType == EWorldType::Type::Game || WorldType == EWorldType::Type::PIE) 
	{
		return true;
	}

	return false;
}

void UGameCurrencySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UGameCurrencySubsystem::Deinitialize()
{
	if (UGameplayMessageSubsystem::HasInstance(this))
	{
		MessageListenerHandle.Unregister();
	}

	Super::Deinitialize();
}

void UGameCurrencySubsystem::OnWorldBeginPlay(UWorld& InWorld) 
{
	// Already registered
	if (MessageListenerHandle.IsValid()) 
	{
		return;
	}

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

	FGameplayMessageListenerParams<FAARewardMessage> Params;
	Params.MatchType = EGameplayMessageMatch::ExactMatch;
	Params.SetMessageReceivedCallback(this, &UGameCurrencySubsystem::ReceivedReward);
	MessageListenerHandle = MessageSubsystem.RegisterListener(GAMEPLAYTAG_Channels_Combat_Rewards, Params);
}

bool UGameCurrencySubsystem::OnSerialize(FSaveGameArchive& Archive, bool bIsLoading)
{
	const bool bSuccess = Archive.SerializeField(TEXT("GameCurrencySubsystem"), [&](FStructuredArchive::FSlot Slot)
		{
			SerializeScriptProperties(Slot);

			if (LOADING)
			{
				// Workaround to prompt it to broadcast the change
				int32 LoadedIntel = CurrentIntel;
				CurrentIntel = 0;
				SetIntel(LoadedIntel, false /* bSendNotifications */);
			}
		});

	return bSuccess;
}

UGameCurrencySubsystem* UGameCurrencySubsystem::Get(const UObject* WorldContext)
{
	if (WorldContext)
	{
		if (const UWorld* World = WorldContext->GetWorld())
		{
			UGameCurrencySubsystem* CurrencySubsystem = World->GetSubsystem<UGameCurrencySubsystem>();
			ensure(CurrencySubsystem);

			return CurrencySubsystem;
		}
	}

	return nullptr;
}

void UGameCurrencySubsystem::ChangeIntel(int Delta, bool bSendNotifications)
{
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

int UGameCurrencySubsystem::GetIntel() const
{
	return CurrentIntel;
}

void UGameCurrencySubsystem::ReceivedReward(FGameplayTag Channel, const FAARewardMessage& Reward)
{
	ChangeIntel(Reward.GainedIntel, true);

	if (Reward.GainedExperience == 0.0f)
	{
		return;
	}

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (Player)
	{
		if (UGameplaySystemComponent* GameplaySystem = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(Player))
		{
			GameplaySystem->AddExperience(Reward.GainedExperience);
		}
	}
}