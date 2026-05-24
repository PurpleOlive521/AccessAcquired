// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/ResponsiveButtonPromptWidget.h"
#include "GameplayTagMessageDefines.h"
#include "GameplayTagDefines.h"

void UResponsiveButtonPromptWidget::NativeDestruct()
{
	UnbindFromInput();

	Super::NativeDestruct();
}

void UResponsiveButtonPromptWidget::AssignContent(const FResponsiveButtonPrompt& Content)
{
	ButtonPrompt = Content;
	K2_RefreshContent(Content);
}

void UResponsiveButtonPromptWidget::BindToInput()
{
	if (UGameplayMessageSubsystem::HasInstance(this))
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

		FGameplayMessageListenerParams<FAAVerbMessage> Params;
		Params.MatchType = EGameplayMessageMatch::ExactMatch;
		Params.SetMessageReceivedCallback(this, &UResponsiveButtonPromptWidget::ReceiveMessage);
		MessageListenerHandle = MessageSubsystem.RegisterListener(GAMEPLAYTAG_Channels_Combat_Player, Params);
	}
}

void UResponsiveButtonPromptWidget::UnbindFromInput()
{
	MessageListenerHandle.Unregister();
}

void UResponsiveButtonPromptWidget::ReceiveMessage(FGameplayTag Channel, const FAAVerbMessage& Message)
{
	if (Message.Verb == ButtonPrompt.TriggeringActionTag)
	{
		K2_OnButtonPromptTriggered(not bHasTriggered);

		bHasTriggered = true;
	}
}

