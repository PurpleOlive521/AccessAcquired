// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayMessageTypes.h"

FString FAAVerbMessage::ToString() const
{
	FString OutString = TEXT("");

	if (Instigator)
	{
		OutString += Instigator->GetName();
	}
	else
	{
		OutString += TEXT("[No Instigator]");
	}

	OutString += TEXT(" ") + Verb.GetTagName().ToString();

	if (Target)
	{
		OutString += Target->GetName();
	}
	else
	{
		OutString += TEXT("[No Target]");
	}

	OutString += TEXT(" with Context: ");

	int i = 1;
	for (FGameplayTag Tag : ContextTags)
	{
		if (i > DEBUG_MAX_CONTEXT_TAGS)
		{
			break;
		}

		OutString += Tag.GetTagName().ToString() + TEXT(", ");

		i++;
	}

	if (ContextTags.Num() > DEBUG_MAX_CONTEXT_TAGS)
	{
		OutString += TEXT("... with %d more tags."), ContextTags.Num() - DEBUG_MAX_CONTEXT_TAGS;
	}

	return OutString;
}