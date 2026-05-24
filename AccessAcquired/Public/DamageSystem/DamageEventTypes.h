// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CombatTypes.h"
#include "GameplayEffect.h"

#include "DamageEventTypes.generated.h"


USTRUCT(BlueprintType)
struct FAADamageEventFlags
{
	GENERATED_BODY()

	FAADamageEventFlags() = default;

	// Will ignore IFrames and deal damage through it. Intended for AOE and DOT effects that generally apply through this.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIgnoreIFrames = false;

	// Forces the target to play a hit animation, if they have one.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bForceHitAnim = false;

	// If we allow the target to play a hit animation. Will be true for most cases, but DOT might want to disable this.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAllowHitAnim = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShowDamageNumbers = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsUnparryable = false;

	// In some cases the instigator will or targer will handle the sound separately, or require no sound at all (e.g. for DOT effects).
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAllowHitSound = true;
};

// Represents an damage event to apply to a target or receive and resolve.
USTRUCT(BlueprintType)
struct FAADamageEvent
{
	GENERATED_BODY()

	FAADamageEvent() = default;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Damage = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AppliedCharge = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Knockback = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDamageType DamageType = EDamageType::EDT_Normal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StaggerDamage = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Level = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FAADamageEventFlags EventFlags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* DamageSource = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector HitLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer ContextTags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UGameplayEffect>> EffectsOnTarget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FAssociatedGameplayEffectStackModifier> StackModifiersOnTarget;
};

// The states that the target is expected to handle as the result of a damage event.
USTRUCT(BlueprintType)
struct FAAResolvedDamageEventStates
{
	GENERATED_BODY()

	FAAResolvedDamageEventStates() = default;
	
	// If the resolver ignored the event, either through statuses like invincibility, i-frames or the target being dead.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bWasEventIgnored = true;

	// If the resolver should stop their current actions & play a hit-animation.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShouldPlayHitAnimation = false;

	// A Overcharge was triggered as a result of this event. Resolved in blueprint for enemies.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bWasOvercharged = false;

	// The resolvers mode was considered a weakness.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bHitWeakness = false;

	// Did a perfect parry occur during this event.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bWasPerfectParried = false;

	// Do we show DamageNumbers representing this event.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShowDamageNumbers = true;

	// In some cases the instigator will or targer will handle the sound separately, or require no sound at all (e.g. for DOT effects).
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShouldPlayHitSound = true;
};

// The result of a damage event, such as damage taken and of which type.
USTRUCT(BlueprintType)
struct FAAResolvedDamageEvent
{
	GENERATED_BODY()

	FAAResolvedDamageEvent() = default;

	FAAResolvedDamageEvent(FAADamageEvent SourceEvent, AActor* InTarget);

	// Configures the event as having been ignored due to the target already being dead.
	void ResolveAsAlreadyDead();

	// Configures the event as having been perfectly dodged by the target.
	void ResolveAsPerfectParried();

	// Configures the event as having been ignored due to the target being untargetable.
	void ResolveAsUntargetable();

	// Configures the event such that the target takes no damage, but still might get interrupted.
	void ResolveAsInvincible(bool bAllowHitAnim);

	// Configures the event such that the target takes damage, but does not get interrupted.
	void ResolveAsUninterruptible();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DamageTaken = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ChargeAdded = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AppliedKnockback = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDamageType DamageType = EDamageType::EDT_Normal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector HitLocation = FVector::ZeroVector;

	// Instigator of the event, e.g. the damage dealer.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<AActor> Source = nullptr;

	// Target of the event, e.g. the damage receiver.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<AActor> Target = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FAAResolvedDamageEventStates EventStates;
};
