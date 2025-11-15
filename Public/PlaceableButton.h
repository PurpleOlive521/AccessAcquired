// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

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

	// --- Start AActor Interface
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// --- End AActor Interface

	// Ensures that the ID is not copied over when the Actor is duplicated in the Editor.
	virtual void PostEditImport() override;

	// --- Begin ISaveableObjectInterface
	virtual void SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject) override;

	virtual void LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject) override;
	// --- End ISaveableObjectInterface

	// --- Begin IInteractableInterface
	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual FInteractionParams GetInteractionData_Implementation() override;
	// --- End IInteractableInterface

	// Get the location to display the Interactive widget from Blueprint 
	UFUNCTION(BlueprintImplementableEvent)
	FVector GetWidgetLocation() const;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Button Pressed"))
	void K2_OnButtonPressed();


protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FInteractionParams InteractionParams;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGuidTag Id;

public:
	UPROPERTY(BlueprintAssignable, Category = "PlaceableButton")
	FOnButtonPressedSignature OnButtonPressedDelegate;
};
