// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "EnemyAsset.h"
#include "EnemyBase.h"

FPrimaryAssetId UEnemyDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("EnemyAssetItems", GetFName());
}

