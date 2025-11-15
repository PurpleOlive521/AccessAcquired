// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "ModeSwitchIconWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameplaySystemComponent.h"
#include "GameplaySystemOwnerInterface.h"

#include "DevCommons.h"


void UModeSwitchIconWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	

	if (IGameplaySystemOwnerInterface* GameplaySystemInterface = Cast<IGameplaySystemOwnerInterface>(Player))
	{
		FGameplayTagSystem* GameplayTagSystem = GameplaySystemInterface->GetGameplayTagSystem();
		GameplayTagSystem->OnGameplayTagChangedDelegate.AddUniqueDynamic(this, &UModeSwitchIconWidget::OnGameplayTagChanged);

		OnGameplayTagChanged(ModeTag, GameplayTagSystem->HasTag(ModeTag) ? 0 : 1);
	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("Player does not have a GameplayTagSystem!"));
	}
}

void UModeSwitchIconWidget::NativeDestruct()
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (Player) // Player might be null on PIE close, which would also trigger destruct on this object
	{
		if (IGameplaySystemOwnerInterface* GameplaySystemInterface = Cast<IGameplaySystemOwnerInterface>(Player))
		{
			FGameplayTagSystem* GameplayTagSystem = GameplaySystemInterface->GetGameplayTagSystem();
			GameplayTagSystem->OnGameplayTagChangedDelegate.RemoveDynamic(this, &UModeSwitchIconWidget::OnGameplayTagChanged);
		}
	}

	Super::NativeDestruct();
}

void UModeSwitchIconWidget::OnGameplayTagChanged(FGameplayTag ChangedTag, int Count)
{
	if (ChangedTag == ModeTag)
	{
		ToggleIconUnlocked(Count > 0);
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
