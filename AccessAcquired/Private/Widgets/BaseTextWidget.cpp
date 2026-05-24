// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "BaseTextWidget.h"

bool UBaseTextWidget::DisplayNextLetter()
{
	if (CurrentLetterIndex >= TargetString.Len() - 1)
	{
		return true;
	}

	DisplayedString += TargetString[CurrentLetterIndex];

	CurrentLetterIndex++;
	return false;
}

void UBaseTextWidget::UpdateNativeSafely(FText NewText)
{
	FString NewTextAsString = NewText.ToString();

	if (NewTextAsString.IsEmpty())
	{
		return;
	}

	int DisplayedStringLength = DisplayedString.Len();
	DisplayedString.Empty(DisplayedStringLength);

	for (int i = 0; i < DisplayedStringLength; i++)
	{
		DisplayedString += NewTextAsString[i];
	}

	TargetString = NewTextAsString;
}

void UBaseTextWidget::ResetInternals()
{
	TargetString = "";
	DisplayedString = "";
	CurrentLetterIndex = 0;
}
