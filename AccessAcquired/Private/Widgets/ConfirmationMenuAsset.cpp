// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/ConfirmationMenuAsset.h"
#include "DevCommons.h"

void FConfirmationMenuContent::ReplaceTokens(const FConfirmationMenuArgs& Arguments)
{
	int32 ArgumentIndex = 0;

	auto FindAndReplaceText = [&](FText& Text)
		{
			FString String = Text.ToString();

			while (true)
			{
				int32 FoundIndex = String.Find(ReplacementToken);

				if (FoundIndex == -1) 
				{
					break;
				}

				String.RemoveAt(FoundIndex, ReplacementToken.Len(), false /* bAllowShrinking */);

				const FString ReplacementString = Arguments.GetStringArgumentAt(ArgumentIndex);

				if (ReplacementString.IsEmpty())
				{
					UE_LOG(VSLog, Warning, TEXT("Could not find replacement string for index '%i' in ConfirmationMenu! Make sure that the amount of arguments match the expected amount."), ArgumentIndex);
				}

				String.InsertAt(FoundIndex, ReplacementString);
				ArgumentIndex++;
			}

			String.Shrink();
			Text = FText::FromString(String);
		};


	FindAndReplaceText(TitleText);
	FindAndReplaceText(BodyText);
	FindAndReplaceText(ConfirmText);
	FindAndReplaceText(CancelText);
}

const float FConfirmationMenuConstants::NO_MAX_DISPLAY_TIME = 0.0f;

FPrimaryAssetId UConfirmationMenuAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("ConfirmationMenuItems", GetFName());
}

FConfirmationMenuContent UConfirmationMenuAsset::GetContent(const FConfirmationMenuArgs& Arguments) const
{
	FConfirmationMenuContent OutContent = Content;
	OutContent.ReplaceTokens(Arguments);

	return OutContent;
}

bool UConfirmationMenuAsset::HasMaxDisplayTime() const
{
	return MaxDisplayTime != FConfirmationMenuConstants::NO_MAX_DISPLAY_TIME;
}

FString FConfirmationMenuArgs::GetStringArgumentAt(int32 Index) const
{
	if (Index >= 0 && Index < Arguments.Num()) 
	{
		return Arguments[Index];
	}

	return FString();
}
