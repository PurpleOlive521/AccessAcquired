// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GameplayAbility.h"
#include "GA_Jump.generated.h"

class ACharacter;

/**
 * 
 */
UCLASS(Abstract)
class VERTICALSLICE_API UGA_Jump : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UGA_Jump();

	// --- Begin UGameplayAbility interface
	virtual bool CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const override;

	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;
	// --- End UGameplayAbility interface

	UFUNCTION(BlueprintCallable, Category = "GA_Jump")
	void StartJump();

	UFUNCTION(BlueprintCallable, Category = "GA_Jump")
	void StopJump();

};
