// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "ScenarioTypes.h"
#include "Encounter.h"

UEncounter* FScenarioEntry::GetEncounter() const
{
	if (!Encounter)
	{
		return nullptr;
	}

	return Encounter;
}
