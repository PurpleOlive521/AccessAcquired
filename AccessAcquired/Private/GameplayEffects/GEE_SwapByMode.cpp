// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayEffects/GEE_SwapByMode.h"

#include "GameplayUtilityBlueprintLibrary.h"
#include "GameplaySystemComponent.h"
#include "ModeComponent.h"

void UGEE_SwapByMode::PreApply(const FGameplayEffectExecutorParams& Params) const
{
	Super::PreApply(Params);

	SwapByMode(Params);
}

void UGEE_SwapByMode::PreReapply(const FGameplayEffectExecutorParams& Params) const
{
	Super::PreReapply(Params);

	if (!bOnlyOnApplication)
	{
		SwapByMode(Params);
	}
}

void UGEE_SwapByMode::SwapByMode(const FGameplayEffectExecutorParams& Params) const
{
	EXPAND_EXECUTOR_PARAMS(Params, GameplaySystem, ActiveEffect, GameplayEffect);

	EDamageType TargetMode = EDamageType::EDT_Normal;

	if (UModeComponent* ModeComponent = GameplaySystem->GetOwner()->GetComponentByClass<UModeComponent>())
	{
		TargetMode = ModeComponent->GetMode();
	}

	TArray<FAttributeEffect> EffectsToRemove;

	for (auto& AttributeEffect : ActiveEffect->AttributeEffects)
	{
		EAttributeType CurrentModeVariation = UGameplayUtilityBlueprintLibrary::ChangeAttributeMode(TargetMode, AttributeEffect.Attribute);

		// No variation exists for Attribute
		if (CurrentModeVariation == EAttributeType::EAT_NONE)
		{
			switch (Policy)
			{
			case ESwapPolicy::ESP_Remove:
				EffectsToRemove.Emplace(AttributeEffect);
				break;
			case ESwapPolicy::ESP_Ignore:
				break;
			}
		}
		else
		{
			AttributeEffect.Attribute = CurrentModeVariation;
		}
	}

	for (auto& AttributeEffectToRemove : EffectsToRemove)
	{
		ActiveEffect->AttributeEffects.Remove(AttributeEffectToRemove);
	}
}
