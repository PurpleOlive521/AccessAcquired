// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScenarioTypes.h"
#include "EncounterTypes.h"
#include "ScenarioUserComponent.generated.h"

class AScenarioVolume;

UCLASS()
class VERTICALSLICE_API UScenarioUserComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UScenarioUserComponent();

	// --- Begin UObject Interface
	virtual void Serialize(FArchive& Ar) override;

	virtual void PostEditImport() override;

	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	// --- End UObject Interface

	// Called when entering a ScenarioVolume
	void OnEnteredScenario(AScenarioVolume* Volume);

	// Called when exiting a ScenarioVolume
	void OnExitedScenario(AScenarioVolume* Volume);

	AScenarioVolume* GetVolume() const;

	void BindToAssets();

	void UnbindFromAssets();

	// An Asset we depend on has been edited, moved or removed.
	void OnAssetInvalidated();

	ScenarioUserId GetUserId() const;

	void SetUserId(ScenarioUserId NewId);

	void SetVolume(AScenarioVolume* Volume);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AScenarioVolume> CurrentVolume = nullptr;

	ScenarioUserId Id = INVALID_SCENARIO_USER_ID;

public:

	// --- Delegates
	FOnAssetChainInvalidatedSignature OnAssetChainInvalidatedDelegate;
};
