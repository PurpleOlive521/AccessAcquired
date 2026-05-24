// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/ResponsiveButtonPromptAsset.h"

FPrimaryAssetId UResponsiveButtonPromptAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("ResponsiveButtonPromptItems", GetFName());
}

FResponsiveButtonPrompt UResponsiveButtonPromptAsset::GetPromptAtIndex(int32 Index)
{
	if (Index >= 0 && Index < ButtonPrompts.Num())
	{
		return ButtonPrompts[Index];
	}

	return FResponsiveButtonPrompt();
}
