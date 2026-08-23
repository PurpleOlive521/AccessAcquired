// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/CoreStatsWidget.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameplaySystemComponent.h"

void UCoreStatsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindToPlayer();
	OnContentChanged();
}

void UCoreStatsWidget::NativeDestruct()
{
	UnbindFromPlayer();

	Super::NativeDestruct();
}

void UCoreStatsWidget::OnContentChanged()
{
	if (UGameplaySystemComponent* GameplaySystem = PlayerGameplaySystem.Get())
	{
		const float RequiredExperience = GameplaySystem->GetExperienceRemainingForNextLevel();
		const float CurrentExperience = GameplaySystem->GetEntityExperience();
		const int32 Level = GameplaySystem->GetEntityLevel();

		K2_OnContentChanged(Level, RequiredExperience, CurrentExperience);
	}
}

void UCoreStatsWidget::OnLevelChanged(int32 NewLevel)
{
	OnContentChanged();
}

void UCoreStatsWidget::OnExperienceChanged(float NewExperience)
{
	OnContentChanged();
}

void UCoreStatsWidget::BindToPlayer()
{
	if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		if (UGameplaySystemComponent* GameplaySystem = UGameplaySystemComponent::GetGameplaySystemFromActor(Player))
		{
			GameplaySystem->OnLevelChangedDelegate.AddUniqueDynamic(this, &UCoreStatsWidget::OnLevelChanged);
			GameplaySystem->OnExperienceChangedDelegate.AddUniqueDynamic(this, &UCoreStatsWidget::OnExperienceChanged);

			PlayerGameplaySystem = MakeWeakObjectPtr(GameplaySystem);
		}
	}
}

void UCoreStatsWidget::UnbindFromPlayer()
{
	if (UGameplaySystemComponent* GameplaySystem = PlayerGameplaySystem.Get())
	{
		GameplaySystem->OnLevelChangedDelegate.RemoveAll(this);
		GameplaySystem->OnExperienceChangedDelegate.RemoveAll(this);
	}
}