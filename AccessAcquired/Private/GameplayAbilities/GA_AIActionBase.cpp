// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayAbilities/GA_AIActionBase.h"

#include "ReceiveDamageEventInterface.h"
#include "DevCommons.h"
#include "DevelopmentTypes.h"


bool UGA_AIActionBase::CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const
{
	FAIL_ON_FAILED_SUPER(CheckAbilityRequirements(ActivationData));

	// No cost attached, don't need a target
	if (ActionTokenCost <= 0)
	{
		return true;
	}

	if (!ActivationData.Target)
	{
		UE_LOG(VSLog, Error, TEXT("GA_AIActionBase: Tried to activate ability without a assigned Target in the ActivationData!"));
		return false;
	}

	if (!ActivationData.Target->Implements<UReceiveDamageEventInterface>())
	{
		UE_LOG(VSLog, Error, TEXT("GA_AIActionBase: Tried to activate ability on a Target that does not implement ReceiveDamageInterface!"));
		return false;
	}

	if (!IReceiveDamageEventInterface::Execute_HasActionTokensAvailable(ActivationData.Target, ActionTokenCost))
	{
		return false;
	}

	return true;
}

bool UGA_AIActionBase::ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData)
{
	FAIL_ON_FAILED_SUPER(ApplyAbilityRequirements(ActivationData));

	if (ActionTokenCost > 0)
	{
		if (!IReceiveDamageEventInterface::Execute_ReserveActionTokens(ActivationData.Target, ActionTokenCost))
		{
			UE_LOG(VSLog, Warning, TEXT("GA_AIActionBase: Could not reserve Action tokens in ApplyAbilityRequirements!"));
			return false;
		}
	}

	return true;
}

void UGA_AIActionBase::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility(ActivationData, OutActiveGameplayAbility);

	Target = ActivationData.Target;
}

void UGA_AIActionBase::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	if (Target)
	{
		IReceiveDamageEventInterface::Execute_ReturnActionTokens(Target, ActionTokenCost);
	}
}

void UGA_AIActionBase::RemoveAbilityEndedModifiers()
{
	Super::RemoveAbilityEndedModifiers();

	if (Target)
	{
		IReceiveDamageEventInterface::Execute_ReserveActionTokens(Target, ActionTokenCost);
	}
}
