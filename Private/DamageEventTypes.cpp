// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#include "DamageEventTypes.h"

FAAResolvedDamageEvent::FAAResolvedDamageEvent(FAADamageEvent SourceEvent)
{
	DamageType = SourceEvent.DamageType;

	HitLocation = SourceEvent.HitLocation;

	DamageSource = SourceEvent.DamageSource;
}
