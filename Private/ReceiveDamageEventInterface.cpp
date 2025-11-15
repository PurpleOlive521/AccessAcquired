#include "ReceiveDamageEventInterface.h"

void FAAResolvedDamageEvent::ResolveAsAlreadyDead()
{
	EventStates.bWasEventIgnored = true;
	EventStates.bShouldPlayHitAnimation = false;
	
	DamageTaken = 0;
	ChargeAdded = 0;
}

void FAAResolvedDamageEvent::ResolveAsPerfectDodged()
{
	EventStates.bWasEventIgnored = true;
	EventStates.bShouldPlayHitAnimation = false;
	EventStates.bWasPerfectDodged = true;

	DamageTaken = 0;
	ChargeAdded = 0;
}

void FAAResolvedDamageEvent::ResolveAsUntargetable()
{
	EventStates.bWasEventIgnored = true;
	EventStates.bShouldPlayHitAnimation = false;

	DamageTaken = 0;
	ChargeAdded = 0;
}

void FAAResolvedDamageEvent::ResolveAsInvincible()
{
	EventStates.bShouldPlayHitAnimation = true;

	DamageTaken = 0;
	ChargeAdded = 0;
}

void FAAResolvedDamageEvent::ResolveAsUninterruptible()
{
	EventStates.bShouldPlayHitAnimation = false;
}
