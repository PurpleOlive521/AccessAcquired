// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GA_AIActionBase.h"
#include "GameplayEffect.h"
#include "GAT_PlayMontageAndWait.h"

#include "GA_AIMeleeAttack.generated.h"

/**
 * Melee Attack ability meant to be used specifically on AIControlled Actors.
 */
UCLASS(Abstract)
class VERTICALSLICE_API UGA_AIMeleeAttack : public UGA_AIActionBase
{
	GENERATED_BODY()

public:

	UGA_AIMeleeAttack();

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GA_AIMeleeAttack")
	TObjectPtr<UAnimMontage> AttackAnimation;

	// Maximum allowed distance from the Target. Will be used during the AI MoveTo pathing.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_AIMeleeAttack")
	float MaxAllowedDistance = 75.0f;
};
