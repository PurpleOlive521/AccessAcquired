// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayEffects/GEE_StayInOverclock.h"
#include "GameplaySystemComponent.h"
#include "GameplayEffects/AttributeEffect.h"

// Ensure that it does go past the Overclock Threshold, and doesn't lie just under it due to float weirdness.
constexpr float COMPENSATION_MARGIN = 0.1f;

void UGEE_StayInOverclock::PostApply(const FGameplayEffectExecutorParams& Params, const FGameplaySystemSnapshot& Snapshot) const
{ 
	Super::PostApply(Params, Snapshot);

	TryStayInOverclock(Params, Snapshot, EGameplayEffectStage::EGES_Apply);
}

void UGEE_StayInOverclock::PostReapply(const FGameplayEffectExecutorParams& Params, const FGameplaySystemSnapshot& Snapshot) const
{
	Super::PostReapply(Params, Snapshot);

	TryStayInOverclock(Params, Snapshot, EGameplayEffectStage::EGES_Reapply);
}

void UGEE_StayInOverclock::TryStayInOverclock(const FGameplayEffectExecutorParams& Params, const FGameplaySystemSnapshot& Snapshot, EGameplayEffectStage Stage) const
{
	EXPAND_EXECUTOR_PARAMS(Params, GameplaySystem, ActiveEffect, GameplayEffect)

	FDelegateCollection::FBroadcastLock Lock = GameplaySystem->OnAttributeChangedDelegateCollection.CreateBroadcastLock();

	const float OverclockThreshold = 0.01f * GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverclockThreshold, EAttributeValue::EAV_CurrentValue);
	const float OverheatLimit = GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverheatLimit, EAttributeValue::EAV_CurrentValue);
	const float Threshold = OverclockThreshold * OverheatLimit;

	const TArray<EAttributeType> _OverheatAttributes = { EAttributeType::EAT_InfraredOverheat, EAttributeType::EAT_UltravioletOverheat, EAttributeType::EAT_GammaOverheat };

	TOptional<FAttributeEffect> CompensatoryEffect;
	for (auto Attribute : _OverheatAttributes)
	{
		CompensatoryEffect = GetCompensatoryEffect(GameplaySystem, Attribute, Snapshot, Threshold);
		if (CompensatoryEffect.IsSet())
		{
			GameplaySystem->ApplyAttributeEffect(CompensatoryEffect.GetValue(), Params.GameplayEffect->DurationType);

			if (Stage == EGameplayEffectStage::EGES_Apply)
			{
				ActiveEffect->AttributeEffects.Add(CompensatoryEffect.GetValue());
			}
		}
	}
}

TOptional<FAttributeEffect> UGEE_StayInOverclock::GetCompensatoryEffect(UGameplaySystemComponent* GameplaySystem, EAttributeType OverheatType, const FGameplaySystemSnapshot& Snapshot, float Threshold) const
{
	const float PreviousOverheat = Snapshot.Attributes.FindRef(OverheatType).CurrentValue;

	if (PreviousOverheat >= Threshold)
	{
		const float NewOverclockThreshold = 0.01f * GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverclockThreshold, EAttributeValue::EAV_CurrentValue);
		const float NewOverheat = GameplaySystem->GetAttributeValue(OverheatType, EAttributeValue::EAV_CurrentValue);
		const float NewOverheatLimit = GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverheatLimit, EAttributeValue::EAV_CurrentValue);

		const float NewThreshold = NewOverclockThreshold * NewOverheatLimit;
		if (NewOverheat < NewThreshold)
		{
			const float Deficit = NewThreshold - NewOverheat + COMPENSATION_MARGIN;
			FAttributeEffect CompensatoryEffect = { OverheatType, Deficit, EEffectApplicationType::EEAT_Addition, ETargetValue::ETV_BaseValue };
			return CompensatoryEffect;
		}
	}

	return TOptional<FAttributeEffect>();
}
