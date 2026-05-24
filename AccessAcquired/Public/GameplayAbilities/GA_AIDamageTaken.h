// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_AIActionBase.h"
#include "GA_AIDamageTaken.generated.h"

class UAIStateLayer_Frozen;
class UGameplayEffect;

/**
 * 
 */
UCLASS(Abstract)
class VERTICALSLICE_API UGA_AIDamageTaken : public UGA_AIActionBase
{
	GENERATED_BODY()

public:

	UGA_AIDamageTaken();

	// --- Begin UGameplayAbility Interface
	virtual bool CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const override;

	virtual bool ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) override;

	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;
	// --- End UGameplayAbility Interface

	// Gets a random montage from HitAnimations.
	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetRandomMontage() const;

	UFUNCTION(BlueprintCallable)
	void OnAnimationFinished();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_AIDamageTaken")
	TArray<TObjectPtr<UAnimMontage>> HitAnimations;

	TObjectPtr<UAIStateLayer_Frozen> FrozenState = nullptr;
	
};
