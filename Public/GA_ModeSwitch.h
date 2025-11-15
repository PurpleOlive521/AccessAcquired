// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GA_PlayMontage.h"
#include "GA_ModeSwitch.generated.h"

enum class EDamageType : uint8;
class UGameplayEffect;

/**
 * Base implementation for player-driven mode switch. Changes the damage type dealt.
 */
UCLASS(Blueprintable)
class VERTICALSLICE_API UGA_ModeSwitch : public UGA_PlayMontage
{
	GENERATED_BODY()
	
public:

	UGA_ModeSwitch();

	virtual bool CheckAbilityRequirements_Implementation() const;

	virtual bool ApplyAbilityRequirements_Implementation();

	virtual void ActivateAbility_Implementation(FActiveGameplayAbility& OutActiveGameplayAbility);

	virtual void ApplyAbilityEndedModifiers_Implementation() override;

	virtual void RemoveAbilityEndedModifiers_Implementation() override;

	virtual void OnMontageBlendingOut(UAnimMontage* Montage, bool bWasInterrupted);

	// Called after the AnimMontage is started.
	UFUNCTION(BlueprintImplementableEvent, Category = "GA_ModeSwitch")
	void OnModeSwitchStarted();

	// Called after the AnimMontage has ended.
	UFUNCTION(BlueprintImplementableEvent, Category = "GA_ModeSwitch")
	void OnModeSwitchEnded(bool bWasInterrupted);



protected:
	// The mode that will be switched to on Ability activation.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_ModeSwitch")
	EDamageType TargetMode;

	// GameplayEffects to apply when the ability is activated with the player having more than 75% Energy.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GA_ModeSwitch")
	TSubclassOf<UGameplayEffect> HighEnergyActivationEffect;


};
