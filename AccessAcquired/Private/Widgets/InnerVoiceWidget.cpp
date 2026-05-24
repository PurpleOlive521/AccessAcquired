// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "InnerVoiceWidget.h"

void UInnerVoiceWidget::ResetInternals_Implementation()
{
	IsPlayingSnippet = false;
	IsDialogueActive = false;
	CurrentSnippetText = "";
}
