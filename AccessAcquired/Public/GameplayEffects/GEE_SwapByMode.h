// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffects/GameplayEffect.h"
#include "GEE_SwapByMode.generated.h"

UENUM(BlueprintType)
enum class ESwapPolicy : uint8
{
	// We don't modify the target attribute.
	ESP_Ignore			UMETA(DisplayName = "Ignore"),

	// We remove the Modifier.
	ESP_Remove		UMETA(DisplayName = "Remove"),
};

/**
 * Swaps Mode-specific attributes to the GameplaySystems owners current Mode. E.g. InfraredOverheat to UltravioletOverheat.
 */
UCLASS()
class VERTICALSLICE_API UGEE_SwapByMode : public UGameplayEffectExecutor
{
	GENERATED_BODY()

public:
	void PreApply(const FGameplayEffectExecutorParams& Params) const override;

	void PreReapply(const FGameplayEffectExecutorParams& Params) const override;

	void SwapByMode(const FGameplayEffectExecutorParams& Params) const;

protected:

	// How we treat Attributes that can't be switched to a Mode variation.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESwapPolicy Policy;

	// Whether we only Swap on application, or also on reapplications.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bOnlyOnApplication = false;
};
