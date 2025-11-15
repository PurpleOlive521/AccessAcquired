// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CombatTypes.h"

#include "DamageEventTypes.generated.h"

class UGameplayEffect;

// Represents an damage event to apply to a target or receive and resolve.
USTRUCT(BlueprintType)
struct FAADamageEvent
{
	GENERATED_BODY()

	FAADamageEvent() {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Damage = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AppliedCharge = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDamageType DamageType = EDamageType::EDT_Normal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DamageMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StaggerDamage = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Level = 0;

	// Will ignore IFrames and deal damage through it. Intended for AOE and DOT effects that generally apply through this.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIgnoreIFrames = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* DamageSource = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector HitLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UGameplayEffect>> EffectsOnTarget;
};

// The states that the target is expected to handle as the result of a damage event.
USTRUCT(BlueprintType)
struct FAAResolvedDamageEventStates
{
	GENERATED_BODY()

	FAAResolvedDamageEventStates() {};
	
	// If the resolver ignored the event, either through statuses like invincibility, i-frames or the target being dead.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bWasEventIgnored = true;

	// If the resolver should stop their current actions & play a hit-animation. Can be triggered by Stagger Damage
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShouldPlayHitAnimation = false;

	// A Overcharge was triggered as a result of this event. Resolved in blueprint for enemies.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bWasOvercharged = false;

	// The resolvers mode was considered a weakness, e.g. >=25% extra damage inflicted.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bHitWeakness = false;

	// Player specific, if the player perfect dodges this event.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bWasPerfectDodged = false;
};

// The result of a damage event, such as damage taken and of which type.
USTRUCT(BlueprintType)
struct FAAResolvedDamageEvent
{
	GENERATED_BODY()

	FAAResolvedDamageEvent() {};

	FAAResolvedDamageEvent(FAADamageEvent SourceEvent);

	// Configures the event as having been ignored due to the target already being dead.
	void ResolveAsAlreadyDead();

	// Configures the event as having been perfectly dodged by the target.
	void ResolveAsPerfectDodged();

	// Configures the event as having been ignored due to the target being untargetable.
	void ResolveAsUntargetable();

	// Configures the event such that the target takes no damage, but still gets interrupted.
	void ResolveAsInvincible();

	// Configures the event such that the target takes damage, but does not get interrupted.
	void ResolveAsUninterruptible();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DamageTaken = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ChargeAdded = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDamageType DamageType = EDamageType::EDT_Normal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector HitLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<AActor> DamageSource = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FAAResolvedDamageEventStates EventStates;
};
