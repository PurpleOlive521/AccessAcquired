// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "ModeSwitchIconWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameplaySystemComponent.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "DevCommons.h"


void UModeSwitchIconWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (IGameplaySystemOwnerInterface* GameplaySystemInterface = Cast<IGameplaySystemOwnerInterface>(Player))
	{
		FGameplayTagSystem* GameplayTagSystem = GameplaySystemInterface->GetGameplayTagSystem();
		GameplayTagHandle = GameplayTagSystem->OnGameplayTagChangedDelegate.AddUObject(this, &UModeSwitchIconWidget::OnGameplayTagChanged);

		FGameplayTag ModeTag = UGameplayUtilityBlueprintLibrary::GetModeAsGameplayTag(Mode);
		OnGameplayTagChanged(ModeTag, GameplayTagSystem->HasTag(ModeTag) ? true : false);
	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("Player does not have a GameplayTagSystem!"));
	}
}

void UModeSwitchIconWidget::NativeDestruct()
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	// Player might be null on PIE close
	if (Player) 
	{
		if (IGameplaySystemOwnerInterface* GameplaySystemInterface = Cast<IGameplaySystemOwnerInterface>(Player))
		{
			FGameplayTagSystem* GameplayTagSystem = GameplaySystemInterface->GetGameplayTagSystem();
			GameplayTagSystem->OnGameplayTagChangedDelegate.Remove(GameplayTagHandle);
		}
	}

	Super::NativeDestruct();
}

void UModeSwitchIconWidget::OnGameplayTagChanged(FGameplayTag ChangedTag, bool bWasAdded)
{
	FGameplayTag ModeTag = UGameplayUtilityBlueprintLibrary::GetModeAsGameplayTag(Mode);
	if (ChangedTag == ModeTag)
	{
		ToggleIconUnlocked(bWasAdded);
	}
}

void UModeSwitchIconWidget::ToggleIconUnlocked(bool bUnlocked)
{
	// Already in that state, exit early
	if (bIsModeUnlocked == bUnlocked)
	{
		return;
	}

	bIsModeUnlocked = bUnlocked;

	if (bIsModeUnlocked)
	{
		SetIconUnlocked();
	}
	else
	{
		SetIconLocked();
	}
}
