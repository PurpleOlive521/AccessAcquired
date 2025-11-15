// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility.h"
#include "GA_Dash.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UGA_Dash : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Dash();
	
	// --- Begin UGameplayAbility interface
	virtual bool CheckAbilityRequirements_Implementation() const override;

	virtual bool ApplyAbilityRequirements_Implementation() override;

	virtual void ActivateAbility_Implementation(FActiveGameplayAbility& OutActiveGameplayAbility) override;

	virtual void CancelAbility_Implementation() override;

	virtual void EndAbility_Implementation() override;

	virtual void ApplyAbilityEndedModifiers_Implementation() override;

	virtual void RemoveAbilityEndedModifiers_Implementation() override;
	// --- End UGameplayAbility interface

	// We wait on activation for the user to give a direction to dash in, before we actually dash by applying forces to the character
	void AfterInputDelay();

	// Called after the input delay. Blueprints opportunity to respond to the activation with VFX & SFX.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Dash Started"))
	void K2_OnDashStarted();

protected:
	
	// The time to wait for user input before activating the dash. Is time-dilation adjusted.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float InputDelay = 0.1f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float DashForceMultiplier = 2.5f;

	// The amount we want to change the camera FOV by.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float CameraFOVDelta = 12.0f;

private:
	FTimerHandle InputDelayTimer;

};
