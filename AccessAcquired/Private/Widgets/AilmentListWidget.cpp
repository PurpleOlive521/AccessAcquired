// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/AilmentListWidget.h"
#include "GameplaySystemComponent.h"
#include "AilmentWidget.h"

void UAilmentListWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAilmentListWidget::NativeDestruct()
{
	UnbindFromGameplaySystem();

	Super::NativeDestruct();
}

void UAilmentListWidget::InitializeWidget(UGameplaySystemComponent* GameplaySystem)
{
	check(GameplaySystem);

	BoundGameplaySystem = MakeWeakObjectPtr(GameplaySystem);

	BindToGameplaySystem();
}

void UAilmentListWidget::ListenForGameplayEffectAdded(TSubclassOf<UGameplayEffect> EffectClass, const FGameplayEffectHandle& AddedHandle)
{
	if (ShouldDisplayGameplayEffect(EffectClass))
	{
		AddAilmentWidget(EffectClass, AddedHandle);
	}
}

void UAilmentListWidget::ListenForGameplayEffectRemoved(const FGameplayEffectHandle& RemovedHandle)
{
	for (auto& [Class, Widget] : GameplayEffectWidgets)
	{
		if (Widget->TrackedHandle == RemovedHandle)
		{
			RemoveAilmentWidget(Class);
			return;
		}
	}
}

void UAilmentListWidget::BindToGameplaySystem()
{
	if (UGameplaySystemComponent* GameplaySystem = BoundGameplaySystem.Get())
	{
		GameplaySystem->OnGameplayEffectAddedDelegate.AddUniqueDynamic(this, &UAilmentListWidget::ListenForGameplayEffectAdded);
		GameplaySystem->OnGameplayEffectRemovedDelegate.AddUniqueDynamic(this, &UAilmentListWidget::ListenForGameplayEffectRemoved);
	}
}

void UAilmentListWidget::UnbindFromGameplaySystem()
{
	if (UGameplaySystemComponent* GameplaySystem = BoundGameplaySystem.Get())
	{
		GameplaySystem->OnGameplayEffectAddedDelegate.RemoveAll(this);
		GameplaySystem->OnGameplayEffectRemovedDelegate.RemoveAll(this);

		BoundGameplaySystem = nullptr;
	}
}

void UAilmentListWidget::AddAilmentWidget(TSubclassOf<UGameplayEffect> EffectClass, const FGameplayEffectHandle& AddedHandle)
{
	if (not GameplayEffectWidgets.Contains(EffectClass))
	{
		UAilmentWidget* CreatedWidget = K2_CreateWidget();

		CreatedWidget->InitializeWidget(BoundGameplaySystem.Get(), AddedHandle);

		GameplayEffectWidgets.Add(EffectClass, CreatedWidget);

		K2_AddAilmentWidget(CreatedWidget);
	}
}

bool UAilmentListWidget::ShouldDisplayGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass)
{
	return DisplayedGameplayEffects.Contains(EffectClass);
}

void UAilmentListWidget::RemoveAilmentWidget(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (TObjectPtr<UAilmentWidget> Widget = GameplayEffectWidgets.FindRef(EffectClass))
	{
		Widget->OnGameplayEffectRemoved();
	}
	
	GameplayEffectWidgets.Remove(EffectClass);

	if(GameplayEffectWidgets.Num() <= 0)
	{
		K2_OnAllAilmentWidgetsRemoved();
	}
}
