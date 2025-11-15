// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "VisualNovelWidget.h"

#include "DevCommons.h"

TSoftObjectPtr<UTexture2D> UVisualNovelWidget::GetPortrait(const TArray<TSubclassOf<UDialogueCharacterData>>& Characters, const FString& Character, const FString& Portrait)
{
	UDialogueCharacterData* CharacterPointer = nullptr;

	// No portrait requested, exit early
	if (Portrait.IsEmpty() || Portrait == "")
	{
		return nullptr;
	}

	// Searching for the CharacterData
	for (TSubclassOf<UDialogueCharacterData> const& It : Characters)
	{
		FString NameCopy = It->GetDefaultObject<UDialogueCharacterData>()->Name;
		UDialogueCharacterData* DefaultObject = It->GetDefaultObject<UDialogueCharacterData>();

		if (DefaultObject->Name == Character)
		{
			CharacterPointer = DefaultObject;
			break;
		}
	}

	// Character not found
	if (CharacterPointer == nullptr)
	{
		UE_LOG(VSLog, Error, TEXT("BaseDialogueWidget: Error - Character not found when trying to get Portrait."));
		return nullptr;
	}

	// Try to get the portrait if its stored as part of the CharacterData and return it
	TSoftObjectPtr<UTexture2D> PortraitPtr = *CharacterPointer->Portraits.Find(Portrait);
	return PortraitPtr;
}

bool UVisualNovelWidget::TypeWriterEffectNative()
{
	// Either the dialogue is empty or we have displayed all letters already
	if (CurrentSnippetText.IsValidIndex(LetterIndex) == false)
	{
		TextToDisplay = CurrentSnippetText;
		IsPlayingSnippet = false;
		return true;
	}

	TCHAR Char = CurrentSnippetText[LetterIndex];
	FString CharAsString = FString(1, &Char);

	// Rich text tag detected
	if (CharAsString == "<" && bIsWritingTag == false)
	{
		FString Tag = "";

		// Grabbing the entire tag as a single string
		for (int i = LetterIndex; i < CurrentSnippetText.Len(); i++)
		{
			Char = CurrentSnippetText[i];
			CharAsString = FString(1, &Char);
			if (CharAsString == ">")
			{
				Tag += CharAsString;
				StartTagEndIndex = i;
				bIsWritingTag = true;
				break;
			}
			else
			{
				Tag += CharAsString;
			}
		}

		// Checking if tag starts with 'img' which is a selfclosing tag
		FString TagStart = Tag.Left(4);

		// Image tag found
		if (TagStart == "<img")
		{
			// Add the complete tag at once and then continue as usual
			TextToDisplay += Tag;
			bIsWritingTag = false;
			LetterIndex += Tag.Len();
			return false;
		}

		// Non image tag, requires closing tag
		TagPrefix = Tag;
		TagTextContent = "";

		// Finding the closing tag
		for (int i = StartTagEndIndex + 1; i < CurrentSnippetText.Len(); i++)
		{
			Char = CurrentSnippetText[i];
			CharAsString = FString(1, &Char);
			if (CharAsString == "<")
			{
				CloseTagStartIndex = i;
				bIsWritingTag = true;
				break;
			}
			else
			{
				TagTextContent += CharAsString;
			}
		}

		TextToDisplayCopy = TextToDisplay;
		// Tag writing setup is done
	}

	if (bIsWritingTag)
	{
		// Check if weve gone through all letters in the tag content yet
		if (StartTagEndIndex + TagTextLetters >= CloseTagStartIndex)
		{
			LetterIndex = CloseTagStartIndex + 3;
			bIsWritingTag = false;
			return false;
		}

		// Getting all the letters of the tags content to currently display
		FString CurrentTagContent = "";
		for (int i = 0; i < TagTextLetters; i++)
		{
			Char = TagTextContent[i];
			CharAsString = FString(1, &Char);
			CurrentTagContent += CharAsString;
		}

		TextToDisplay = TextToDisplayCopy + TagPrefix + CurrentTagContent + TagSuffix;
		TagTextLetters++;

		return false;
	}

	// No tag found
	TextToDisplay += CharAsString;
	LetterIndex++;
	return false;
}

void UVisualNovelWidget::ResetInternals_Implementation()
{
	TextToDisplay = "";
	TextToDisplayCopy = "";
	TagPrefix = "";
	TagTextContent = "";

	LetterIndex = 0;
	StartTagEndIndex = 0;
	CloseTagStartIndex = 0;

	bIsWritingTag = false;
	TagTextLetters = 1;
}
