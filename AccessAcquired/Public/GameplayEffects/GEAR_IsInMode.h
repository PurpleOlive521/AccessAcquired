// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffects/GameplayEffect.h"
#include "ReceiveDamageEventInterface.h"
#include "GEAR_IsInMode.generated.h"

/**
 * Requires that the target of the GameplayEffect is in the specified mode to be applied.
 * Can be inverted to require the opposite.
 */
UCLASS()
class VERTICALSLICE_API UGEAR_IsInMode : public UGameplayEffectCondition
{
	GENERATED_BODY()
	
public:

	bool CanApply(const FGameplayEffectConditionParams& Params) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EDamageType RequiredMode = EDamageType::EDT_Normal;

	// Inverts the result, requiring that we are NOT in the mode to apply instead.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bInvertValue = false;

};
