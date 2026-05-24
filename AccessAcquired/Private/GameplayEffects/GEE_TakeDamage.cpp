// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GEE_TakeDamage.h"
#include "ReceiveDamageEventInterface.h"
#include "GameplaySystemComponent.h"
#include "DevCommons.h"

void UGEE_TakeDamage::PreApply(const FGameplayEffectExecutorParams& Params) const
{
	Super::PreApply(Params);

	if (StagesToApply.Contains(EGameplayEffectStage::EGES_Apply))
	{
		TryDealDamage(Params);
	}
}

void UGEE_TakeDamage::PreReapply(const FGameplayEffectExecutorParams& Params) const
{
	Super::PreReapply(Params);

	if (StagesToApply.Contains(EGameplayEffectStage::EGES_Reapply))
	{
		TryDealDamage(Params);
	}
}

void UGEE_TakeDamage::PreRemove(const FGameplayEffectExecutorParams& Params) const
{
	Super::PreRemove(Params);

	EXPAND_EXECUTOR_PARAMS(Params, GameplaySystem, ActiveEffect, GameplayEffect);

	if (StagesToApply.Contains(EGameplayEffectStage::EGES_Remove))
	{
		if (bApplyOnlyOnForceRemovals)
		{
			if (not ActiveEffect->IsRemoveExternal())
			{
				return;
			}
		}

		TryDealDamage(Params);
	}
}

void UGEE_TakeDamage::TryDealDamage(const FGameplayEffectExecutorParams& Params) const
{
	EXPAND_EXECUTOR_PARAMS(Params, GameplaySystem, ActiveEffect, GameplayEffect);

	AActor* Owner = GameplaySystem->GetOwner();

	const bool bImplementsInterface = Owner->Implements<UReceiveDamageEventInterface>();

	float CalculatedDamage = GameplaySystem->CalculateCoefficientAttribute(DamageCalculation);

	if (bAccountForStacks)
	{
		if (GameplayEffect->IsStackingEffect() && ActiveEffect->IsFullyApplied())
		{
			CalculatedDamage *= ActiveEffect->AppliedStacks;
		}
	}

	if (bRouteThroughDamageInterface)
	{
		if (!bImplementsInterface)
		{
			UE_LOG(VSLog, Warning, TEXT("GEE_TakeDamage: Could not route GGE_TakeDamage through interface!"));
			return;
		}

		FAADamageEvent Event;
		Event.DamageSource = ActiveEffect->Instigator.Get();
		Event.Damage = CalculatedDamage;
		Event.EventFlags = DamageEventProperties;
		Event.HitLocation = Owner->GetActorLocation() + FVector(0.0f, 0.0f, 80.0f); // Hardcoded offset - most characters are taller than their location, so we move it up a bit
		Event.DamageType = DamageType;

		IReceiveDamageEventInterface::Execute_ReceiveDamageEvent(Owner, Event);
	}
	else
	{
		GameplaySystem->ModifyAttributeValue(EAttributeType::EAT_Health, EAttributeValue::EAV_BaseValue, -CalculatedDamage);
	}

}
