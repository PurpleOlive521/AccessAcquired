// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "EncounterTypes.h"
#include "EnemyAsset.h"

UEnemyDataAsset* FEncounterEntry::GetEnemyAsset() const
{
	if (EnemyAsset.IsNull())
	{
		return nullptr;
	}

	return EnemyAsset.LoadSynchronous();
}
