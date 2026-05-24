// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AIActionBase.h"
#include "GA_Overcharged.generated.h"

class UAIStateLayer_Frozen;

/**
 * 
 */
UCLASS(Abstract)
class VERTICALSLICE_API UGA_Overcharged : public UGA_AIActionBase
{
	GENERATED_BODY()
	
public:

	UGA_Overcharged();

	// --- Begin UGameplayAbility interface
	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;

	virtual void EndAbility() override;

	virtual void CancelAbility() override;
	// --- End UGameplayAbility interface

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_Overcharged")
	TObjectPtr<UAnimMontage> EnterOverchargedMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_Overcharged")
	TObjectPtr<UAnimMontage> OverchargedLoopMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_Overcharged")
	TObjectPtr<UAnimMontage> RecoveredMontage;

	TObjectPtr<UAIStateLayer_Frozen> FrozenState = nullptr;
};
