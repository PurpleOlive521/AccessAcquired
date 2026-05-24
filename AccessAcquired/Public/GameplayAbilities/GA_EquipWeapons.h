// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility.h"
#include "GAT_PlayMontageAndWait.h"
#include "GA_EquipWeapons.generated.h"

class UWeaponComponent;

/**
 * 
 */
UCLASS(Abstract)
class VERTICALSLICE_API UGA_EquipWeapons : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UGA_EquipWeapons();

	// --- Begin UGameplayAbility interface
	virtual bool CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const override;

	virtual void ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void ApplyAbilityEndedModifiers() override;

	virtual void RemoveAbilityEndedModifiers() override;
	// --- End UGameplayAbility interface

	UFUNCTION(BlueprintCallable)
	FPlayMontageAndWaitParams GetPlayMontageParams() const;

	UFUNCTION(BlueprintCallable)
	void OnAnimationFinished(bool bWasInterrupted);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_EquipWeapons")
	TObjectPtr<UAnimMontage> EquipAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_EquipWeapons")
	float MinRandomPlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_EquipWeapons")
	float MaxRandomPlayRate = 1.0f;

	// Treats it as the animation having gone through even if cancelled. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_EquipWeapons")
	bool bForceEquippedStateOnCancel = true;

private:
	TObjectPtr<UWeaponComponent> WeaponComp = nullptr;
};
