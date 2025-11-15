// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GA_PlayMontage.h"
#include "GameplayEffect.h"

#include "GA_MeleeAttack.generated.h"

class UGameplayEffect;


/**
 * Base implementation for player-driven melee attacks. 
 */
UCLASS(Blueprintable)
class VERTICALSLICE_API UGA_MeleeAttack : public UGA_PlayMontage
{
	GENERATED_BODY()

public:

	UGA_MeleeAttack();

	// --- Begin UGameplayAbility interface
	virtual bool CheckAbilityRequirements_Implementation() const override;

	virtual bool ApplyAbilityRequirements_Implementation() override;

	virtual void ActivateAbility_Implementation(FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void CancelAbility_Implementation() override;

	virtual void ApplyAbilityEndedModifiers_Implementation() override;

	virtual void RemoveAbilityEndedModifiers_Implementation() override;

	virtual FString ToString() const override;

	virtual FString ToStringWithDebugTags() const override;
	// --- End UGameplayAbility interface

	// --- Begin UGA_PlayMontage interface
	virtual void OnMontageBlendingOut(UAnimMontage* Montage, bool bWasInterrupted) override;
	// --- End UGA_PlayMontage interface

	// Called after the AnimMontage is started.
	UFUNCTION(BlueprintImplementableEvent, Category = "GA_MeleeAttack")
	void OnAttackStarted();

	// Called after the AnimMontage has ended.
	UFUNCTION(BlueprintImplementableEvent, Category = "GA_MeleeAttack")
	void OnAttackEnded(bool bWasInterrupted);


protected:

	// The amount of Energy required to activate the ability
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameplayAbility")
	float EnergyCost = 0;

	// 2.0f is double the damage, 0.5f is half the damage etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Units = "Times"), Category = "GA_MeleeAttack")
	float DamageMultiplier = 1.0f;

	// If this value is above the Stagger Threshold, stagger the entity (hit animation)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_MeleeAttack")
	float StaggerDamage = 100.0f;

	// Applied once the attack animation has started
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_MeleeAttack")
	TArray<TSubclassOf<UGameplayEffect>> AttackStartedEffects;

	// Applied once the attack animation has ended
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_MeleeAttack")
	TArray<TSubclassOf<UGameplayEffect>> AttackEndedEffects;

	TArray<FGameplayEffectHandle> AttackEndedEffectHandles;

	// Logs information about the motion warping process, such as Actors found, filtered and targeted.
	UPROPERTY(EditDefaultsOnly, Category = "GameplayAbility|Debug")
	bool bEnableDebugLogs = false;

};
