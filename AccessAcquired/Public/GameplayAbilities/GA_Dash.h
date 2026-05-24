// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility.h"
#include "GA_Dash.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class VERTICALSLICE_API UGA_Dash : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Dash();
	
	// --- Begin UGameplayAbility interface
	virtual bool CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const override;

	virtual bool ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) override;

	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void CancelAbility() override;

	virtual void EndAbility() override;

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;
	// --- End UGameplayAbility interface

	UFUNCTION(BlueprintCallable, Category = "GA_Dash")
	void EnableGravity();

protected:
	
};
