// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractableInterface.h"
#include "WC_InteractionPrompt.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UWC_InteractionPrompt : public UWidgetComponent
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, Category = "InteractionPrompt")
	void OnInteract();

	// A new interactable has been focused.
	UFUNCTION(BlueprintImplementableEvent, Category = "InteractionPrompt")
	void OnSwitchedInteractable();

	// Only called when there are no other interactables to focus.
	UFUNCTION(BlueprintImplementableEvent, Category = "InteractionPrompt")
	void OnLeftInteractable();

	UFUNCTION(BlueprintImplementableEvent, Category = "InteractionPrompt")
	void SetContent(const FInteractionParams& Params);
};
