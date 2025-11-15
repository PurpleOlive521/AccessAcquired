// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "DialogueSystemTypes.h"
#include "DialogueWidgetInterface.h"
#include "ExtendedCommonActivatableWidget.h"
		  
#include "BaseDialogueWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueFinishedSignature, FString, DialogueId);

UCLASS(Blueprintable)
class VERTICALSLICE_API UBaseDialogueWidget : public UExtendedCommonActivatableWidget, public IDialogueWidgetInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite)
	TArray<FPackagedDialogueSegment> QueuedDialogue;

	UPROPERTY(BlueprintReadWrite)
	FPackagedDialogueSegment CurrentDialogue;

	UPROPERTY(BlueprintReadWrite)
	TArray<FDialogueRow> DialogueHistory;

	// Whether a snippets text is being typed out to display
	UPROPERTY(BlueprintReadWrite)
	bool IsPlayingSnippet = false;

	// Whether the UI is currently displaying dialogue
	UPROPERTY(BlueprintReadWrite)
	bool IsDialogueActive = false;

	UPROPERTY(BlueprintReadWrite)
	int CurrentSnippetIndex = -1;

	UPROPERTY(BlueprintReadWrite)
	FString CurrentSnippetText = "";

public:

	// Starts the next piece of dialogue containing any number of lines. Restarts most internal logic keeping track of the state.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void NextDialogue();

	// Prompts the next line to be shown on the screen
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void NextSnippet();

	// Handles displaying the content of the new snippet
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DisplaySnippet(const FDialogueRow& DialogueToDisplay);

	// Adds a dialogue to be played. Call before starting dialogue
	virtual void QueueDialogue_Implementation(const FPackagedDialogueSegment& PackagedDialogue) override;

	// Meant to be called before displaying a new snippet.
	// Clears any values that kept track of the previous snippets state.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ResetInternals();

	// Base implementation
	virtual void ResetInternals_Implementation();

	UFUNCTION(BlueprintCallable)
	void RecordSnippet(const FDialogueRow& SnippetToRecord);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, BlueprintReadWrite)
	FOnDialogueFinishedSignature OnDialogueFinishedDelegate;
};
