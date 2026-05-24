// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayAbility.h"
#include "GameplayEffect.h"
#include "GAT_PlayMontageAndWait.h"

#include "GA_ChargedHeavyAttack.generated.h"

class UDamageEventModifier;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UGA_ChargedHeavyAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_ChargedHeavyAttack();

	// --- Begin UGameplayAbility Interface
	virtual bool CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const override;

	virtual bool ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) override;

	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void CancelAbility() override;

	virtual void EndAbility() override;

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;
	// --- End UGameplayAbility Interface

	UFUNCTION(BlueprintCallable, Category = "GA_ChargedHeavyAttack")
	FPlayMontageAndWaitParams GetPlayMontageParams() const;

	// Sets damage values based on the time charged. Expects a 0 to 1 value for ChargingProgress.
	UFUNCTION(BlueprintCallable, Category = "GA_ChargedHeavyAttack")
	void OnChargingFinished(float TimeCharged);

	// Starts counting the progress breakpoints.
	UFUNCTION(BlueprintCallable)
	void OnChargingStarted();

	UFUNCTION()
	void OnChargingBreakpointReached();
	
	// Called when a TrackedChargingBreakpoints is reached. Allows Blueprint to manage VFX as the charging progresses.
	// Is 1 indexed. 
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Charging Breakpoint Reached"), Category = "GA_ChargedHeavyAttack")
	void K2_OnChargingBreakpointReached(int32 Point);

	void WaitForNextBreakpoint();

	void RemoveModifiers();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_ChargedHeavyAttack")
	TObjectPtr<UAnimMontage> AttackAnimation;

	// The maximum achievable multiplier from charging. Is scaled by the charging progress if released early.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Units = "Times"), Category = "GA_ChargedHeavyAttack")
	float MaxMultiplier = 5.0f;

	// Format each point as a fraction, e.g 0.25 for a breakpoint to trigger after 25% of the MaxChargingTime.  
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_ChargedHeavyAttack")
	TArray<float> TrackedChargingBreakpoints;

	// The max time that we reward for. Any further charging will not affect damage output or VFX.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_ChargedHeavyAttack")
	float MaxRewardedChargingTime = 2.5f;

	// We automatically end the charging after this point.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_ChargedHeavyAttack")
	float MaxTotalChargingTime = 5.0f;

	// Applied once the charging animation has started. Removed when the ability ends.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_ChargedHeavyAttack")
	TArray<TSubclassOf<UGameplayEffect>> ChargeEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_ChargedHeavyAttack")
	TSubclassOf<UDamageEventModifier> DamageModifier = nullptr;

	TArray<FGameplayEffectHandle> ChargeEffectHandles;

private:

	int32 CurrentBreakpoint = 0;
};
