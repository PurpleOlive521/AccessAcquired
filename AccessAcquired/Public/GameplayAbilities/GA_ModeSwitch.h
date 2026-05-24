// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility.h"
#include "CombatTypes.h"
#include "GA_ModeSwitch.generated.h"

/**
 * Base implementation for player-driven mode switch. Changes the damage type dealt.
 */
UCLASS(Abstract)
class VERTICALSLICE_API UGA_ModeSwitch : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UGA_ModeSwitch();

	// --- Begin UGameplayAbility Interface
	virtual bool CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const;

	virtual bool ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData);

	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility);

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;
	// --- End UGameplayAbility Interface

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_ModeSwitch")
	TObjectPtr<UAnimMontage> ModeAnimation;

	// The mode that will be switched to on Ability activation.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_ModeSwitch")
	EDamageType TargetMode;
};
