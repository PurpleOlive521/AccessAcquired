// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffects/GameplayEffect.h"
#include "GEAR_BlockByTags.generated.h"

UENUM(BlueprintType)
enum class EApplicationRequirementPolicy : uint8
{
	EARP_All			UMETA(DisplayName = "All"),				// Requirement is enforced on both Apply and Reapply
	EARP_ApplyOnly		UMETA(DisplayName = "Apply Only"),		// Requirement is only enforced on Apply
	EARP_ReapplyOnly	UMETA(DisplayName = "Reapply Only"),	// Requirement is only enforced on Reapply
};

/**
 * Requires that the target not have any of the tags that we specify in BlockingTags. 
 * More granular conditions can be used to only check per Apply or Reapply respectively.
 */
UCLASS()
class VERTICALSLICE_API UGEAR_BlockByTags : public UGameplayEffectCondition
{
	GENERATED_BODY()
	
public:

	bool CanApply(const FGameplayEffectConditionParams& Params) const override;

	bool CanReapply(const FGameplayEffectConditionParams& Params) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EApplicationRequirementPolicy Policy = EApplicationRequirementPolicy::EARP_All;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagContainer BlockingTags;
};
