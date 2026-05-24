// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayEffects/GES_Ailment.h"
#include "GameplaySystemComponent.h"

void UGES_Ailment::PreApplyStackModifier(const FGameplayEffectStackerParams& Params, FGameplayEffectStackModifier& StackModifier) const
{
	Super::PreApplyStackModifier(Params, StackModifier);

	EXPAND_STACKER_PARAMS(Params, GameplaySystem, ActiveEffect, GameplayEffect);

	const float AilmentResistance = GameplaySystem->GetAttributeValue(EAttributeType::EAT_AilmentResistance, EAttributeValue::EAV_CurrentValue);
	const float AilmentImmunity = GameplaySystem->GetAttributeValue(EAttributeType::EAT_AilmentImmunity, EAttributeValue::EAV_CurrentValue);
	const float FinalAilmentImmunity = FMath::Abs(1.0f - (AilmentImmunity / 100.0f));

	const float TotalStackProgressLimit = GameplayEffect->StackProgressLimit * AilmentResistance;

	ActiveEffect->SetStackProgressLimit(TotalStackProgressLimit, GameplaySystem);

	StackModifier.StackProgress *= FinalAilmentImmunity;
}

void UGES_Ailment::PostApplyStackModifier(const FGameplayEffectStackerParams& Params, const FGameplayEffectStackModifier& StackModifier) const
{
	EXPAND_STACKER_PARAMS(Params, GameplaySystem, ActiveEffect, GameplayEffect);

	Super::PostApplyStackModifier(Params, StackModifier);
}
