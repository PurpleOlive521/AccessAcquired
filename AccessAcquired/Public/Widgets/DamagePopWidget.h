// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageEventTypes.h"
#include "DamagePopWidget.generated.h"

class UDamagePopContainerWidget;
class UCanvasPanelSlot;
class UCanvasPanel;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UDamagePopWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void Setup(UDamagePopContainerWidget* InOwningContainer);

	// Returns true if the Widget is not playing.
	bool SetAndPlay(const FAAResolvedDamageEvent& DamageEvent, UCanvasPanel* Canvas);

	// Moves the widget over HitLocation in viewport-space.
	UFUNCTION(BlueprintCallable)
	void ProjectOverHitLocation();

	bool IsActive() const;

	FVector GetWorldLocation() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Set And Play"))
	void K2_SetAndPlay();

	// Call from BP to end the DamagePopup and return it to it's pool.
	UFUNCTION(BlueprintCallable)
	void FinishPlaying();

	UPROPERTY(BlueprintReadWrite)
	FVector2D DesiredAlignment = { 0.5f, 0.5f };

	UPROPERTY(BlueprintReadWrite)
	UCanvasPanelSlot* ParentSlot = nullptr;

	uint32 bIsActive : 1 = false;

	UPROPERTY(BlueprintReadOnly)
	FAAResolvedDamageEvent DamageEvent;

	TWeakObjectPtr<UDamagePopContainerWidget> OwningContainer = nullptr;
};
