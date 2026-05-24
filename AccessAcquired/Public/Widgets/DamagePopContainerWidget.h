// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "DamagePopContainerWidget.generated.h"

class UDamagePopWidget;

// The amount of widgets past WidgetPoolSize we allow before we start decaying them.
constexpr int DECAY_SIZE_BUFFER = 10;

/**
 * Widget that manages and pools DamagePopUpWidgets. Widgets are consumed when receiving DamageEvents.
 * Will allocate to always meet demand, which will then be decayed when the widgets are returned.
 */
UCLASS()
class VERTICALSLICE_API UDamagePopContainerWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// --- Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	// --- End UUserWidget Interface

	void Setup();

	// MessageSubsystem Callback
	UFUNCTION()
	void ReceiveDamageEvent(FGameplayTag Channel, const FAADamageEventMessage& EventMessage);

	// Consumes the widget.
	[[nodiscard]] UDamagePopWidget* GetAvailableWidget();

	// Creates a new DamagePopupWidget and adds it to the pool.
	UDamagePopWidget* CreateWidget();

	// Creates a new DamagePopupWidget and immediately consumes it.
	UDamagePopWidget* CreateAndConsumeWidget();

	void ReturnWidget(UDamagePopWidget* Widget);

	// Hook for native to get the root panel that DamagePopups will be placed in.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Root Widget"))
	UCanvasPanel* K2_GetRootWidget();

	bool HasActiveWidgetCloseToLocation(FVector WorldLocation);

	FVector GetRandomOffset() const;

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamagePopWidget> WidgetClass;

	// If the distance between recent received events is less than this value, add a randomised offset to it.
	UPROPERTY(EditDefaultsOnly)
	float DistanceRandomisationThreshold = 30.0f;

	// World-space offset distance.
	UPROPERTY(EditDefaultsOnly)
	float RandomisedOffsetDistance = 30.0f;

	// The amount of DamagePopupWidgets to pre-allocate.
	UPROPERTY(EditDefaultsOnly)
	int WidgetPoolSize = 20;

	UPROPERTY()
	TArray<UDamagePopWidget*> WidgetPool;

	// Indices into WidgetPool that are currently available for use.
	TArray<int> AvailableWidgets; 

private:

	FGameplayMessageListenerHandle MessageHandle;

};
