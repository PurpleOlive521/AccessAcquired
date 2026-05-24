// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "HitboxAsset.h"


FPrimaryAssetId UHitboxAsset::GetPrimaryAssetId() const
{
    return FPrimaryAssetId("HitboxAssetItems", GetFName());
}

FHitboxParams::FHitboxParams()
{
    HitboxType.AddTag(GAMEPLAYTAG_Hitboxes_Types_Weapon);
}
