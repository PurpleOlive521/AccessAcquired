// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AIActionBase.h"
#include "GA_OverchargedRecovery.generated.h"

class UAIStateLayer_Frozen;

/**
 * Use combined GA_Overcharged for this entire sequence instead!
 */
UCLASS(Deprecated)
class VERTICALSLICE_API UDEPRECATED_GA_OverchargedRecovery : public UGA_AIActionBase
{
	GENERATED_BODY()
	
public:

	UDEPRECATED_GA_OverchargedRecovery();

	// --- Begin UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;
	// --- End UGameplayAbility Interface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_OverchargedRecovery")
	TObjectPtr<UAnimMontage> RecoverMontage;

	TObjectPtr<UAIStateLayer_Frozen> FrozenState = nullptr;
};
