// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "AttributeTypes.h"
#include "OverheatBarsWidget.generated.h"

class UGameplaySystemComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UOverheatBarsWidget : public UUserWidget
{
	GENERATED_BODY()
	
	enum class EForcedVisibility : uint8
	{
		Visible,
		Hidden,
		None,
	};

public:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void ListenForOverheat(EAttributeType Attribute);

	void ReceiveMessage(FGameplayTag Channel, const FAAVerbMessage& Message);
	
	UFUNCTION(BlueprintCallable, Category = "OverheatBarsWidget")
	void ModifyVisibilityCount(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "OverheatBarsWidget")
	void ForceWidgetVisibility(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "OverheatBarsWidget")
	void ClearForcedVisibility();

	UFUNCTION(BlueprintCallable, Category = "OverheatBarsWidget")
	void SetIsMovedAside(bool bInIsMovedAside);

	// Called when we want to show the widget.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Show Widget"), Category = "OverheatBarsWidget")
	void K2_ShowWidget();

	// Called when we want to hide the widget.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Hide Widget"), Category = "OverheatBarsWidget")
	void K2_HideWidget();

	// Called when we want the widget to move to the side, to give space to other UI elements.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Move Aside Widget"), Category = "OverheatBarsWidget")
	void K2_MoveAsideWidget();

	// Called when we want the widget to move back to it's original position.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Move Back Widget"), Category = "OverheatBarsWidget")
	void K2_MoveBackWidget();

protected:

	void OnVisibilityChanged();

	void SetForcedVisibility(EForcedVisibility State);

	void SetCompoundVisibility(bool bShouldBeVisible);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Percent"), Category = "OverheatBarsWidget")
	float OverheatVisiblityThreshold = 40.0f;

	EForcedVisibility ForcedVisibility = EForcedVisibility::None;

	bool bInfraredOverThreshold = false;
	bool bUltravioletOverThreshold = false;
	bool bGammaOverThreshold = false;
	
	bool bCompoundIsVisible = false;

	bool bIsMovedAside = false;

	// Widget should be visible when count is positive and hidden if count is 0 or negative.
	int32 VisiblityCount = 0;

	FGameplayMessageListenerHandle MessageHandle;

	TWeakObjectPtr<UGameplaySystemComponent> BoundGameplaySystem = nullptr;
};
