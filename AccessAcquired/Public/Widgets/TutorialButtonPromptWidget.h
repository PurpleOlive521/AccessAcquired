// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayMessages/GameplayMessageTypes.h"
#include "ResponsiveButtonPromptAsset.h"
#include "TutorialButtonPromptWidget.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UTutorialButtonPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void AssignContent(UResponsiveButtonPromptAsset* Content);

	UFUNCTION(BlueprintCallable, Category = "TutorialButtonPromptWidget")
	void HideWidget();

	// Blueprints opportunity to display any requested content from Content.
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "Refresh Content"), Category = "TutorialButtonPromptWidget")
	void K2_RefreshContent(UResponsiveButtonPromptAsset* Content);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Hide Widget"), Category = "TutorialButtonPromptWidget")
	void K2_HideWidget();

	UFUNCTION()
	void ReceiveMessage(FGameplayTag Channel, const FAATutorialButtonPromptMessage& Message);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "TutorialButtonPromptWidget")
	TObjectPtr<UResponsiveButtonPromptAsset> ContentAsset;

	bool bIsWidgetDisplayed = false;

	FGameplayMessageListenerHandle MessageListenerHandle;
};
