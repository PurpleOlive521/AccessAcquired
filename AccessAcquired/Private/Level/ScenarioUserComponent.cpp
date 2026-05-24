// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Level/ScenarioUserComponent.h"
#include "Scenario.h"
#include "ScenarioVolume.h"
#include "ScenarioCustomVersion.h"
#include "DevCommons.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"

UScenarioUserComponent::UScenarioUserComponent()
{
}

void UScenarioUserComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FScenarioCustomVersion::GUID);

	if (Ar.IsLoading() && Ar.CustomVer(FScenarioCustomVersion::GUID) < FScenarioCustomVersion::InitialCustomProperties)
	{
		// No custom serialization added yet, don't try to load 
		return;
	}

	if (Ar.IsLoading())
	{
		// Since CurrentVolume is a UPROPERTY, it might be loaded silently and won't be bound through OnEnteredScenario so we need to force it here
		BindToAssets();
	}

	Ar << Id;
}

void UScenarioUserComponent::PostEditImport()
{
	CurrentVolume = nullptr;
}

void UScenarioUserComponent::PostDuplicate(bool bDuplicateForPIE)
{
	if (!bDuplicateForPIE)
	{
		CurrentVolume = nullptr;
	}
}

void UScenarioUserComponent::OnEnteredScenario(AScenarioVolume* Volume)
{
	check(Volume);

	if (CurrentVolume == Volume)
	{
		return;
	}

	if (CurrentVolume)
	{

#if WITH_EDITOR
		// This might be triggered by a race-condition, prompt other volumes search first
		CurrentVolume->RequestImmediateSearch();

		if (CurrentVolume)
		{
			UE_LOG(VSLog, Warning, TEXT("ScenarioUserComponent: Entered multiple ScenarioVolumes (%s)! Do not overlap ScenarioVolumes."), *Volume->GetActorLabel());
			return;
		}
#endif //WITH_EDITOR

		UE_LOG(VSLog, Warning, TEXT("ScenarioUserComponent: Entered multiple ScenarioVolumes (%s)! Do not overlap ScenarioVolumes."), *Volume->GetName());
		return;
	}

	SetVolume(Volume);

	// Fetch new Id
	if (Id == INVALID_SCENARIO_USER_ID)
	{
		SetUserId(CurrentVolume->GetNextParticipantId());
	}
}

void UScenarioUserComponent::OnExitedScenario(AScenarioVolume* Volume)
{
	check(Volume);

	if (CurrentVolume == Volume)
	{
		SetUserId(INVALID_SCENARIO_USER_ID);
		SetVolume(nullptr);
	}
}

AScenarioVolume* UScenarioUserComponent::GetVolume() const
{
	return CurrentVolume;
}

void UScenarioUserComponent::BindToAssets()
{
	if (CurrentVolume)
	{
		CurrentVolume->OnAssetChainInvalidatedDelegate.AddUObject(this, &UScenarioUserComponent::OnAssetInvalidated);
	}
}

void UScenarioUserComponent::UnbindFromAssets()
{
	if (CurrentVolume)
	{
		CurrentVolume->OnAssetChainInvalidatedDelegate.RemoveAll(this);
	}
}

void UScenarioUserComponent::OnAssetInvalidated()
{
	OnAssetChainInvalidatedDelegate.Broadcast();
}

ScenarioUserId UScenarioUserComponent::GetUserId() const
{
	return Id;
}

void UScenarioUserComponent::SetUserId(ScenarioUserId NewId)
{
	if (Id == NewId)
	{
		return;
	}

	if (CurrentVolume)
	{
		CurrentVolume->ReturnId(Id);
	}

	Id = NewId;

	OnAssetInvalidated();
}

void UScenarioUserComponent::SetVolume(AScenarioVolume* Volume)
{
	if (CurrentVolume == Volume)
	{
		return;
	}

	// In case we are still bound to the old Volume
	UnbindFromAssets();

	CurrentVolume = Volume;

	BindToAssets();

	OnAssetInvalidated();
}
