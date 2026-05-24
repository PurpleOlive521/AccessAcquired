// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffects/GameplayEffect.h"
#include "DamageEventTypes.h"
#include "GEE_TakeDamage.generated.h"

/**
 * Executor that applies damage to the owning GameplaySystem of the applied GameplayEffect.
 */
UCLASS()
class VERTICALSLICE_API UGEE_TakeDamage : public UGameplayEffectExecutor
{
	GENERATED_BODY()
	
public:
	void PreApply(const FGameplayEffectExecutorParams& Params) const override;

	void PreReapply(const FGameplayEffectExecutorParams& Params) const override;

	void PreRemove(const FGameplayEffectExecutorParams& Params) const override;

	void TryDealDamage(const FGameplayEffectExecutorParams& Params) const;

	// The damage value we will deal.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FCoefficientAttribute DamageCalculation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bRouteThroughDamageInterface"))
	FAADamageEventFlags DamageEventProperties;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bRouteThroughDamageInterface"))
	EDamageType DamageType = EDamageType::EDT_Normal;

	// The different events where we apply damage.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EGameplayEffectStage> StagesToApply;

	// We only trigger on Remove if the GameplayEffect was removed from an external source, and not from duration elapsing.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bApplyOnlyOnForceRemovals = false;

	// If false, will try to apply it through the GameplaySystem itself. This might conceal it from important callbacks such as VFX & SFX.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRouteThroughDamageInterface = true;

	// If the triggering Effect is stacking, multiply the dealt damage by the amount of stacks present.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAccountForStacks = false;
};
