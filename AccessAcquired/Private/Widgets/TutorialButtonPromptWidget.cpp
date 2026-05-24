// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/TutorialButtonPromptWidget.h"
#include "GameplayTagMessageDefines.h"

void UTutorialButtonPromptWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameplayMessageSubsystem::HasInstance(this))
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

		FGameplayMessageListenerParams<FAATutorialButtonPromptMessage> Params;
		Params.MatchType = EGameplayMessageMatch::ExactMatch;
		Params.SetMessageReceivedCallback(this, &UTutorialButtonPromptWidget::ReceiveMessage);
		MessageListenerHandle = MessageSubsystem.RegisterListener(GAMEPLAYTAG_Channels_GameplayEvents_Tutorial, Params);
	}
}

void UTutorialButtonPromptWidget::NativeDestruct()
{
	MessageListenerHandle.Unregister();

	Super::NativeDestruct();
}

void UTutorialButtonPromptWidget::AssignContent(UResponsiveButtonPromptAsset* Content)
{
	if (not Content)
	{
		return;
	}

	bIsWidgetDisplayed = true;

	K2_RefreshContent(Content);
}

void UTutorialButtonPromptWidget::HideWidget()
{
	if (not bIsWidgetDisplayed)
	{
		return;
	}

	bIsWidgetDisplayed = false;

	K2_HideWidget();
}

void UTutorialButtonPromptWidget::ReceiveMessage(FGameplayTag Channel, const FAATutorialButtonPromptMessage& Message)
{
	if (Message.bShouldRemoveDisplay)
	{
		HideWidget();
		return;
	}

	if (Message.ContentAsset)
	{
		AssignContent(Message.ContentAsset);
	}
}
