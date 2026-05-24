// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffects/GameplayEffect.h"
#include "GameplayEffects/AttributeEffect.h"
#include "GEE_StayInOverclock.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UGEE_StayInOverclock : public UGameplayEffectExecutor
{
	GENERATED_BODY()
	
public:

	void PostApply(const FGameplayEffectExecutorParams& Params, const FGameplaySystemSnapshot& Snapshot) const override;

	void PostReapply(const FGameplayEffectExecutorParams& Params, const FGameplaySystemSnapshot& Snapshot) const override;

	void TryStayInOverclock(const FGameplayEffectExecutorParams& Params, const FGameplaySystemSnapshot& Snapshot, EGameplayEffectStage Stage) const;

	// Tries to get a compensatory effect to keep the Overheat above the OverclockThreshold, only if this GameplayEffect caused us to go under the threshold.
	TOptional<FAttributeEffect> GetCompensatoryEffect(UGameplaySystemComponent* GameplaySystem, EAttributeType OverheatType, const FGameplaySystemSnapshot& Snapshot, float Threshold) const;

};
