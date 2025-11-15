// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "ReinforcedDestructible.h"

FAAResolvedDamageEvent AReinforcedDestructible::ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent)
{
	// Ignore damage events that do not match the mode of this destructible
	if (DamageEvent.DamageType != Mode)
	{
		return FAAResolvedDamageEvent(DamageEvent);
	}

	// Call parent implementation
	FAAResolvedDamageEvent ParentResolvedEvent = Super::ReceiveDamageEvent_Implementation(DamageEvent);

	return ParentResolvedEvent;
}
