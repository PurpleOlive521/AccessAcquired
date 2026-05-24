// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Scenario.h"
#include "Encounter.h"

void UScenario::PostLoad()
{
	Super::PostLoad();

	BindToAssets();
}

#if WITH_EDITOR
void UScenario::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UnbindFromAssets();
	BindToAssets();

	OnAssetInvalidated();
}
#endif //WITH_EDITOR

FPrimaryAssetId UScenario::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("ScenarioAssetItems", GetFName());
}

UEncounter* UScenario::GetCurrentEncounter() const
{
	return Entry.GetEncounter();
}

void UScenario::BindToAssets()
{
	if (UEncounter* Encounter = GetCurrentEncounter())
	{
		Encounter->OnAssetChainInvalidatedDelegate.AddUObject(this, &UScenario::OnAssetInvalidated);
	}
}

void UScenario::UnbindFromAssets()
{
	if (UEncounter* Encounter = GetCurrentEncounter())
	{
		Encounter->OnAssetChainInvalidatedDelegate.RemoveAll(this);
	}
}

void UScenario::OnAssetInvalidated()
{
	OnAssetChainInvalidatedDelegate.Broadcast();
}
