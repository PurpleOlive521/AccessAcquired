// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "BaseDialogueWidget.h"

#include "VisualNovelWidget.generated.h"

UCLASS(Blueprintable)
class VERTICALSLICE_API UVisualNovelWidget : public UBaseDialogueWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite)
	FString TextToDisplay = "";

	int LetterIndex = 0;

	// Tag-writing specific members
	bool bIsWritingTag = false;
	FString TagPrefix = "";
	FString TagSuffix = "</>";
	FString TextToDisplayCopy = "";
	FString TagTextContent = "";
	int TagTextLetters = 1;

	int StartTagEndIndex = 0;
	int CloseTagStartIndex = 0;

public:

	// Gets the the portrait of the character requested from an array of CharacterData
	UFUNCTION(BlueprintCallable)
	TSoftObjectPtr<UTexture2D> GetPortrait(const TArray<TSubclassOf<UDialogueCharacterData>>& Characters, const FString& Character, const FString& Portrait);

	// Returns true when its done displaying the complete snippet
	UFUNCTION(BlueprintCallable)
	bool TypeWriterEffectNative();

	virtual void ResetInternals_Implementation() override;

};
