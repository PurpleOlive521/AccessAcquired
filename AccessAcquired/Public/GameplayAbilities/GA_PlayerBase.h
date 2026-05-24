// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayAbility.h"
#include "AttributeTypes.h"

#include "GA_PlayerBase.generated.h"


// Base class for abilities that might require Overheat to activate. Slight misnomer, since it now supports any Actor with a ModeComponent.
UCLASS(Blueprintable)
class VERTICALSLICE_API UGA_PlayerBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_PlayerBase();

	// --- Begin UGameplayAbility Interface
	virtual bool CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const;

	virtual bool ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData);

	virtual FString ToString() const override;

	virtual FString ToStringWithDebugTags() const override;
	// --- End UGameplayAbility Interface

	// The amount of Overheat required in the current mode to activate the ability
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameplayAbility|Costs")
	float OverheatCost = 0;

};
