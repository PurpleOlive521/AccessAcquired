// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SaveableObjectInterface.h"
#include "GuidTag.h"
#include "InteractableInterface.h"

#include "DialogueInteractable.generated.h"

UCLASS()
class VERTICALSLICE_API ADialogueInteractable : public AActor, public ISaveableObjectInterface, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	ADialogueInteractable();

	// --- Begin AActor Interface
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

protected:

	UFUNCTION(BlueprintCallable)
	void SetInteractableState(bool bInInteractedWithPreviously);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Interactable State Updated"))
	void K2_SetInteractableState(bool bInInteractedWithPreviously);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGuidTag Id;

	UPROPERTY(BlueprintReadWrite)
	bool bInteractedWithPreviously = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FInteractionParams InteractionParams;


private:
	TObjectPtr<USceneComponent> WidgetLocationComponent;


};
