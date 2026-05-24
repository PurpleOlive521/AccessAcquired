// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility.h"
#include "GA_AIActionBase.generated.h"

/**
 * Base class for AI Actions that require Action Tokens to be performed, usually playing a AnimMontage when activated.
 * The Action Tokens are reserved when activating the ability, and are required for it to activate.
 * All reserved Tokens are returned when the ability ends.
 */
UCLASS(Abstract)
class VERTICALSLICE_API UGA_AIActionBase : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_AIActionBase() = default;

	// --- Begin UGameplayAbility Interface
	virtual bool CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const override;

	virtual bool ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) override;

	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;

	// --- End UGameplayAbility Interface

protected:

	// The amount of Action Tokens that are required on the Target to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_AIActionBase")
	int ActionTokenCost = 0;

	// The target that we reserved Action Tokens from.
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Target = nullptr;
};
