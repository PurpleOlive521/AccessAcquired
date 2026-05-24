// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility.h"
#include "GA_DamageTaken.generated.h"

class UGameplayEffect;

/**
 * 
 */
UCLASS(Abstract)
class VERTICALSLICE_API UGA_DamageTaken : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_DamageTaken();

	// --- Begin UGameplayAbility Interface
	virtual bool ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) override;

	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;
	// --- End UGameplayAbility Interface
	
	// Gets a random montage from HitAnimations.
	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetRandomMontage(const FRotator& Direction) const;

	const TArray<UAnimMontage*>& GetAnimationBucket(const FRotator& Direction) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_DamageTaken")
	TArray<TObjectPtr<UAnimMontage>> ForwardAnimations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_DamageTaken")
	TArray<TObjectPtr<UAnimMontage>> BackwardsAnimations;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_DamageTaken")
	TArray<TObjectPtr<UAnimMontage>> LeftAnimations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_DamageTaken")
	TArray<TObjectPtr<UAnimMontage>> RightAnimations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_DamageTaken")
	bool bPlayHitAnimation = false;
};
