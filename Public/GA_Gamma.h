// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GA_PlayerBase.h"
#include "GA_Gamma.generated.h"

UCLASS(Blueprintable)
class VERTICALSLICE_API UGA_Gamma : public UGA_PlayerBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameplayAbility")
	float GlobalTimeSlow = 0.1f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GameplayAbility")
	bool bIsEffectActive = false;

public:
	void ActivateAbility_Implementation(FActiveGameplayAbility& OutActiveGameplayAbility) override;

	void EndAbility_Implementation() override;

	// Activates the time slow for everything but the player
	UFUNCTION(BlueprintCallable, Category = "GameplayAbility")
	void ActivateEffect();

	// Resets the time slow for everything
	UFUNCTION(BlueprintCallable, Category = "GameplayAbility")
	void EndEffect();


};
