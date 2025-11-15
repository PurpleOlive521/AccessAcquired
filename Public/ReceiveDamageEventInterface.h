// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CombatTypes.h"
#include "DamageEventTypes.h"

#include "ReceiveDamageEventInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UReceiveDamageEventInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Allows any Actor to receive damage events, and resolve them internally.
 * Implementing the AttackToken functions allows AI to target the Actor.
 */
class VERTICALSLICE_API IReceiveDamageEventInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageEvents")
	FAAResolvedDamageEvent ReceiveDamageEvent(FAADamageEvent DamageEvent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageEvents")
	bool IsDead();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageEvents")
	ETeam GetTeam();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageEvents")
	bool ReserveAttackToken(int Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageEvents")
	void ReturnAttackToken(int Amount);

	// Returns true if Amount at minimum is available.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageEvents")
	bool HasAttackTokensAvailable(int Amount) const;
};
