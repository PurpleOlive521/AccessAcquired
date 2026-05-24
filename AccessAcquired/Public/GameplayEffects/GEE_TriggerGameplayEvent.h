// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffects/GameplayEffectExecutor.h"
#include "GEE_TriggerGameplayEvent.generated.h"

class UGameplayEvent;

/**
 * Triggers GameplayEvents during key GameplayEffect events.
 */
UCLASS()
class VERTICALSLICE_API UGEE_TriggerGameplayEvent : public UGameplayEffectExecutor
{
	GENERATED_BODY()
	
public:

	virtual void PostApply(const FGameplayEffectExecutorParams& Params, const FGameplaySystemSnapshot& Snapshot) const override;

	virtual void PostRemove(const FGameplayEffectExecutorParams& Params, const FGameplaySystemSnapshot& Snapshot) const override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEvent>> OnApplyEvents;

	// Aborts any GameplayEvents triggered on application when the GameplayEffect is removed.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAbortApplyEventsOnRemove = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEvent>> OnRemoveEvents;
};
