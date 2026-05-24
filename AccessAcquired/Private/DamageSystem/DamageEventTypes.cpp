// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "DamageEventTypes.h"

FAAResolvedDamageEvent::FAAResolvedDamageEvent(FAADamageEvent SourceEvent, AActor* InTarget)
{
	DamageType =						SourceEvent.DamageType;
	HitLocation =						SourceEvent.HitLocation;
	Source =							SourceEvent.DamageSource;
	Target =							InTarget;
	EventStates.bShowDamageNumbers =	SourceEvent.EventFlags.bShowDamageNumbers;
	EventStates.bShouldPlayHitSound =	SourceEvent.EventFlags.bAllowHitSound;
}

void FAAResolvedDamageEvent::ResolveAsAlreadyDead()
{
	EventStates.bWasEventIgnored = true;
	EventStates.bShouldPlayHitAnimation = false;
	EventStates.bShowDamageNumbers = false;

	DamageTaken = 0.0f;
	ChargeAdded = 0.0f;
	AppliedKnockback = 0.0f;
}

void FAAResolvedDamageEvent::ResolveAsPerfectParried()
{
	EventStates.bWasEventIgnored = true;
	EventStates.bShouldPlayHitAnimation = false;
	EventStates.bWasPerfectParried = true;
	EventStates.bShowDamageNumbers = false;

	DamageTaken = 0.0f;
	ChargeAdded = 0.0f;
	AppliedKnockback = 0.0f;
}

void FAAResolvedDamageEvent::ResolveAsUntargetable()
{
	EventStates.bWasEventIgnored = true;
	EventStates.bShouldPlayHitAnimation = false;
	EventStates.bShowDamageNumbers = false;

	DamageTaken = 0.0f;
	ChargeAdded = 0.0f;
	AppliedKnockback = 0.0f;
}

void FAAResolvedDamageEvent::ResolveAsInvincible(bool bAllowHitAnim)
{
	EventStates.bWasEventIgnored = false;

	if (bAllowHitAnim)
	{
		EventStates.bShouldPlayHitAnimation = true;
	}

	DamageTaken = 0.0f;
	ChargeAdded = 0.0f;
}

void FAAResolvedDamageEvent::ResolveAsUninterruptible()
{
	EventStates.bShouldPlayHitAnimation = false;
	AppliedKnockback = 0.0f;
}

