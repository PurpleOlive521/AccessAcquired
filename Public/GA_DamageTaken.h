// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilities/GA_PlayMontage.h"
#include "GA_DamageTaken.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UGA_DamageTaken : public UGA_PlayMontage
{
	GENERATED_BODY()

public:

	UGA_DamageTaken();

	// --- Begin UGameplayAbility interface
	virtual bool ApplyAbilityRequirements_Implementation() override;

	virtual void ActivateAbility_Implementation(FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void ApplyAbilityEndedModifiers_Implementation() override;

	virtual void RemoveAbilityEndedModifiers_Implementation() override;
	// --- End UGameplayAbility interface

	// --- Begin UGA_PlayMontage interface
	virtual void OnMontageBlendingOut(UAnimMontage* Montage, bool bWasInterrupted) override;
	// --- End UGA_PlayMontage interface

	// Blueprints opportunity to activate VFX & SFX when this ability is activated.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Damage Taken"), Category = "GA_DamageTaken")
	void K2_PlayActivationEffects();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_DamageTaken")
	TArray<TObjectPtr<UAnimMontage>> HitAnimations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_DamageTaken")
	TArray<TSubclassOf<UGameplayEffect>> OnActivationEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_DamageTaken")
	bool bPlayHitAnimation = false;
};
