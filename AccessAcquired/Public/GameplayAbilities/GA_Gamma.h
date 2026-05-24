// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GA_PlayerBase.h"
#include "GA_Gamma.generated.h"

UCLASS(Blueprintable)
class VERTICALSLICE_API UGA_Gamma : public UGA_PlayerBase
{
	GENERATED_BODY()

public:
	void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility) override;

	void EndAbility() override;
};
