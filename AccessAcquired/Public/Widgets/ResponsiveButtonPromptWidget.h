// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResponsiveButtonPromptAsset.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayMessages/GameplayMessageTypes.h"
#include "ResponsiveButtonPromptWidget.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UResponsiveButtonPromptWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "ResponsiveButtonPromptWidget")
	void AssignContent(const FResponsiveButtonPrompt& Content);

	// Blueprints opportunity to display any requested content from Content.
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "Refresh Content"), Category = "ResponsiveButtonPromptWidget")
	void K2_RefreshContent(const FResponsiveButtonPrompt& Content);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "On Button Prompt Triggered"), Category = "ResponsiveButtonPromptWidget")
	void K2_OnButtonPromptTriggered(bool bIsFirstTime);

	UFUNCTION(BlueprintCallable, Category = "ResponsiveButtonPromptWidget")
	void BindToInput();

	UFUNCTION(BlueprintCallable, Category = "ResponsiveButtonPromptWidget")
	void UnbindFromInput();

	UFUNCTION()
	void ReceiveMessage(FGameplayTag Channel, const FAAVerbMessage& Message);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "ResponsiveButtonPromptWidget")
	FResponsiveButtonPrompt ButtonPrompt;

	FGameplayMessageListenerHandle MessageListenerHandle;

	bool bHasTriggered = false;
};
