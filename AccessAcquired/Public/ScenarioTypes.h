// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ScenarioTypes.generated.h"

class UEncounter;

typedef uint32 ScenarioUserId;

constexpr uint32 INVALID_SCENARIO_USER_ID = 0U;

USTRUCT(BlueprintType)
struct VERTICALSLICE_API FScenarioEntry
{
	GENERATED_BODY()

	FScenarioEntry() = default;

	UEncounter* GetEncounter() const;

	UPROPERTY(EditAnywhere, Category = "ScenarioEntry")
	TObjectPtr<UEncounter> Encounter = nullptr;
};