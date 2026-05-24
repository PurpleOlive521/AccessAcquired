// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SaveableObjectInterface.h"
#include "GuidTag.h"
#include "InteractableInterface.h"

#include "PlaceableButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonPressedSignature);

UCLASS()
class VERTICALSLICE_API APlaceableButton : public AActor, public ISaveableObjectInterface, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	APlaceableButton();

	// --- Begin IInteractableInterface
	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual FInteractionParams GetInteractionData_Implementation() override;
	// --- End IInteractableInterface

	// --- Begin SaveableObject Interface
	virtual bool OnSerialize(FSaveGameArchive& Archive, bool bIsLoading) override;
	// --- End SaveableObject Interface

	// Get the location to display the Interactive widget from Blueprint 
	UFUNCTION(BlueprintImplementableEvent)
	FVector GetWidgetLocation() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Button Pressed"))
	void K2_OnButtonPressed();


protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FInteractionParams InteractionParams;

public:
	UPROPERTY(BlueprintAssignable, Category = "PlaceableButton")
	FOnButtonPressedSignature OnButtonPressedDelegate;
};
