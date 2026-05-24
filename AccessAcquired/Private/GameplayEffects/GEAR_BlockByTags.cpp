// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayEffects/GEAR_BlockByTags.h"
#include "GameplaySystemComponent.h"
#include "DevelopmentTypes.h"

bool UGEAR_BlockByTags::CanApply(const FGameplayEffectConditionParams& Params) const
{
	FAIL_ON_FAILED_SUPER(CanApply(Params));

	if (Policy == EApplicationRequirementPolicy::EARP_All || Policy == EApplicationRequirementPolicy::EARP_ApplyOnly)
	{
		if (Params.GameplaySystem->GetGameplayTagSystem()->HasAnyTag(BlockingTags))
		{
			return false;
		}
	}

	return true;
}

bool UGEAR_BlockByTags::CanReapply(const FGameplayEffectConditionParams& Params) const
{
	FAIL_ON_FAILED_SUPER(CanReapply(Params));

	if (Policy == EApplicationRequirementPolicy::EARP_All || Policy == EApplicationRequirementPolicy::EARP_ReapplyOnly)
	{
		if (Params.GameplaySystem->GetGameplayTagSystem()->HasAnyTag(BlockingTags))
		{
			return false;
		}
	}

	return true;
}
