// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "WeaponAttachmentAsset.h"

FPrimaryAssetId UWeaponAttachmentAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("WeaponAttachmentAssetItems", GetFName());
}
