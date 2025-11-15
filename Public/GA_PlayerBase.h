// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayAbility.h"

#include "GA_PlayerBase.generated.h"


// Base class for abilities that are only able to be performed by the player, or that relies on player-only functionality.
UCLASS(Blueprintable)
class VERTICALSLICE_API UGA_PlayerBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_PlayerBase();

	// --- Begin UGameplayAbility interface
	virtual bool CheckAbilityRequirements_Implementation() const;

	virtual bool ApplyAbilityRequirements_Implementation();

	virtual FString ToString() const override;

	virtual FString ToStringWithDebugTags() const override;
	// --- End UGameplayAbility interface


protected:

	// The amount of Energy required to activate the ability
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameplayAbility|Costs")
	float EnergyCost = 0;
};
