// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType, meta = (ScriptName = "AA_AISense"))
enum class EAISense : uint8
{
	AS_None				UMETA(DisplayName = "None"),
	AS_Sight			UMETA(DisplayName = "Sight"),
	AS_Damage			UMETA(DisplayName = "Damage"),
	AS_Proximity		UMETA(DisplayName = "Proximity"),
	AS_Hearing			UMETA(DisplayName = "Hearing"),
};

struct VERTICALSLICE_API FKnownTargetData
{
	FKnownTargetData() = default;

	FKnownTargetData(AActor* InTarget);

	AActor* GetTarget() const;

	TWeakObjectPtr<AActor> Target;

	float Priority = 0.0f;

	friend uint32 GetTypeHash(const FKnownTargetData& InHandle);

	bool operator==(const FKnownTargetData& Other) const
	{
		return Target == Other.Target;
	}

	bool operator!=(const FKnownTargetData& Other) const
	{
		return Target != Other.Target;
	}
};
