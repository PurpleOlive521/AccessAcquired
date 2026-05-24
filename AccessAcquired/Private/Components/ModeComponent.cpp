// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "ModeComponent.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "GameplaySystemComponent.h"

UModeComponent::UModeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UModeComponent::BeginPlay()
{
	Super::BeginPlay();
}

UModeComponent* UModeComponent::GetModeComponentFromActor(const AActor* Actor)
{
	if (not Actor)
	{
		return nullptr;
	}

	return Actor->FindComponentByClass<UModeComponent>();
}

EDamageType UModeComponent::GetModeFromActor(const AActor* Actor)
{
	if (UModeComponent* ModeComponent = GetModeComponentFromActor(Actor))
	{
		return ModeComponent->GetMode();
	}

	return EDamageType::EDT_Normal;
}

void UModeComponent::SetMode(EDamageType NewMode)
{
	if (CurrentMode == NewMode)
	{
		return;
	}

	if (!HasMode(NewMode))
	{
		return;
	}

	CurrentMode = NewMode;

	OnModeSwitchedDelegate.Broadcast(CurrentMode);
}

EDamageType UModeComponent::GetMode() const
{
	return CurrentMode;
}

bool UModeComponent::HasMode(EDamageType Mode)
{
	const FGameplayTag ModeAvailabilityTag = UGameplayUtilityBlueprintLibrary::GetModeAsGameplayTag(Mode);

	FGameplayTagSystem* GameplayTagSystem = GetGameplaySystem()->GetGameplayTagSystem();
	if (!GameplayTagSystem)
	{
		return false;
	}

	return GameplayTagSystem->HasTag(ModeAvailabilityTag);
}

UGameplaySystemComponent* UModeComponent::GetGameplaySystem()
{
	if (GameplaySystem.IsExplicitlyNull())
	{
		UGameplaySystemComponent* Comp = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(GetOwner());
		check(Comp);
		GameplaySystem = MakeWeakObjectPtr(Comp);
	}

	return GameplaySystem.Get();
}
