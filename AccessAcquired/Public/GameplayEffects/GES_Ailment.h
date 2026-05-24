// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffects/GameplayEffectStacker.h"
#include "GES_Ailment.generated.h"

/**
 * Ailments incorporate a couple Attributes that define the StackProgress behaviour, giving Ailments a unique behaviour with stacking.
 * This exposes the StackProgress and stacking system to gameplay properties and allows for different Actors to be susceptible to different Ailments.
 */
UCLASS()
class VERTICALSLICE_API UGES_Ailment : public UGameplayEffectStacker
{
	GENERATED_BODY()
	
public:

	virtual void PreApplyStackModifier(const FGameplayEffectStackerParams& Params, FGameplayEffectStackModifier& StackModifier) const override;

	virtual void PostApplyStackModifier(const FGameplayEffectStackerParams& Params, const FGameplayEffectStackModifier& StackModifier) const override;
};
