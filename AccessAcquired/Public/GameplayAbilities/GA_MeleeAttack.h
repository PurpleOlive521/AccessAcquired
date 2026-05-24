// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayAbility.h"
#include "GameplayEffect.h"
#include "GAT_PlayMontageAndWait.h"
#include "EnhancedInputComponent.h"

#include "GA_MeleeAttack.generated.h"

/**
 * Base implementation for player-driven melee attacks. 
 * Sets the Ability's Duration to the length of the played animation if the Duration is 0.
 */
UCLASS(Abstract)
class VERTICALSLICE_API UGA_MeleeAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_MeleeAttack();

	// --- Begin UGameplayAbility Interface
	virtual bool CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const override;

	virtual bool ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) override;

	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void CancelAbility() override;

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;
	// --- End UGameplayAbility Interface

	UFUNCTION(BlueprintCallable)
	FPlayMontageAndWaitParams GetPlayMontageParams() const;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_MeleeAttack")
	TObjectPtr<UAnimMontage> AttackAnimation = nullptr;
};
