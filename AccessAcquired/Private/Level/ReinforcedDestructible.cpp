// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "ReinforcedDestructible.h"

FAAResolvedDamageEvent AReinforcedDestructible::ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent)
{
	// Ignore damage events that do not match the mode of this destructible
	if (DamageEvent.DamageType != Mode)
	{
		return FAAResolvedDamageEvent(DamageEvent, this);
	}

	// Call parent implementation
	FAAResolvedDamageEvent ParentResolvedEvent = Super::ReceiveDamageEvent_Implementation(DamageEvent);

	return ParentResolvedEvent;
}
