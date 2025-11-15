// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


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
