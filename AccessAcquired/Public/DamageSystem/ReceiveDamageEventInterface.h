// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

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
 * Implementing the ActionTokens functions allows AI to target the Actor and perform special actions towards them.
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
	bool ReserveActionTokens(int Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageEvents")
	void ReturnActionTokens(int Amount);

	// Returns true if Amount at minimum is available.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DamageEvents")
	bool HasActionTokensAvailable(int Amount) const;
};
