// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "BaseDialogueWidget.h"

void UBaseDialogueWidget::ResetInternals_Implementation()
{
	IsPlayingSnippet = false;
	IsDialogueActive = false;
	CurrentSnippetText = "";
}

void UBaseDialogueWidget::RecordSnippet(const FDialogueRow& SnippetToRecord)
{
	DialogueHistory.Add(SnippetToRecord);
}

void UBaseDialogueWidget::QueueDialogue_Implementation(const FPackagedDialogueSegment& PackagedDialogue)
{
	QueuedDialogue.Add(PackagedDialogue);
}
