// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GameplayAbility.h"
#include "GA_PerfectDodge.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UGA_PerfectDodge : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UGA_PerfectDodge();

	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility);

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;
};
