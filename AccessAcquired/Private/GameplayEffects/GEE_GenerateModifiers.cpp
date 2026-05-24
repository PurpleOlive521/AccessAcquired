// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayEffects/GEE_GenerateModifiers.h"

#include "GameplayUtilityBlueprintLibrary.h"
#include "GameplaySystemComponent.h"

void UGEE_GenerateModifiers::PreApply(const FGameplayEffectExecutorParams& Params) const
{
	Super::PreApply(Params);

	GenerateModifiers(Params);
}

void UGEE_GenerateModifiers::GenerateModifiers(const FGameplayEffectExecutorParams& Params) const
{
	EXPAND_EXECUTOR_PARAMS(Params, GameplaySystem, ActiveEffect, GameplayEffect);

	TArray<EDamageType> TargetModes = ModesToGenerate;


	if (bAllModesButCurrent)
	{
		EDamageType PlayerMode = UGameplayUtilityBlueprintLibrary::GetPlayerMode(GameplaySystem);
		TargetModes = CombatConstants::AllModes;
		TargetModes.Remove(PlayerMode);
	}

	TArray<FAttributeEffect> Templates;

	for (auto& AttributeEffect : ActiveEffect->AttributeEffects)
	{
		if (!UGameplayUtilityBlueprintLibrary::IsModeAttribute(AttributeEffect.Attribute))
		{
			continue;
		}
		
		Templates.Emplace(AttributeEffect);
	}

	for (auto& Template : Templates)
	{
		ActiveEffect->AttributeEffects.Remove(Template);

		for (EDamageType Mode : TargetModes)
		{
			const EAttributeType NewType = UGameplayUtilityBlueprintLibrary::ChangeAttributeMode(Mode, Template.Attribute);

			if (NewType != EAttributeType::EAT_NONE)
			{
				FAttributeEffect GeneratedEffect = Template;
				GeneratedEffect.Attribute = NewType;
				ActiveEffect->AttributeEffects.Emplace(GeneratedEffect);
			}
		}
	}
}