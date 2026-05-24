// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffects/GameplayEffect.h"
#include "DamageEventTypes.h"
#include "GEE_GenerateModifiers.generated.h"

/**
 * Requires that the Template Modifiers that we generate copies from are an Mode attribute (e.g. GammaWeakness) and of the same Mode. 
 * Do not mix Mode attributes, such as having both InfraredOverheat AND UltravioletOverheat.
 */
UCLASS()
class VERTICALSLICE_API UGEE_GenerateModifiers : public UGameplayEffectExecutor
{
	GENERATED_BODY()
	
public:
	void PreApply(const FGameplayEffectExecutorParams& Params) const override;

	void GenerateModifiers(const FGameplayEffectExecutorParams& Params) const;

protected:

	// Generates Modifiers for all modes but the one the Player is currently in.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAllModesButCurrent = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "!bAllModesButCurrent"))
	TArray<EDamageType> ModesToGenerate;
};
