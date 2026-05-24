// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SaveableObjectInterface.h"
#include "GuidTag.h"
#include "InteractableInterface.h"

#include "DialogueInteractable.generated.h"

USTRUCT(BlueprintType)
struct FDialogueCameraPoint
{
	GENERATED_BODY()


	// In Level-space. Is converted when used with sublevels.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DialogueCameraPoint")
	bool bPointAtSelf = false;

	// In Level-space. Is converted when used with sublevels.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DialogueCameraPoint")
	FVector Offset = FVector::ZeroVector;

	// In Level-space. Is converted when used with sublevels.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bPointAtSelf == false"), Category = "DialogueCameraPoint")
	FVector Point = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bPointAtSelf == false"), Category = "DialogueCameraPoint")
	TSoftObjectPtr<AActor> TargetActor = nullptr;
};

UCLASS()
class VERTICALSLICE_API ADialogueInteractable : public AActor, public ISaveableObjectInterface, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ADialogueInteractable();

	virtual void BeginPlay() override;

	// --- Begin SaveableObject Interface
	virtual bool OnSerialize(FSaveGameArchive& Archive, bool bIsLoading) override;
	// --- End SaveableObject Interface

	// --- Begin IInteractableInterface
	virtual FInteractionParams GetInteractionData_Implementation() override;
	// --- End IInteractableInterface

	UFUNCTION(BlueprintCallable, Category = "DialogueInteractable")
	void SetInteractableState(bool bInInteracted);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Interactable State Updated"), Category = "DialogueInteractable")
	void K2_SetInteractableState(bool bInInteracted);

protected:

	UPROPERTY(BlueprintReadWrite, SaveGame, Category = "DialogueInteractable")
	bool bInteracted = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DialogueInteractable")
	FInteractionParams InteractionParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DialogueInteractable")
	FDialogueCameraPoint CameraPoint;

private:
	TObjectPtr<USceneComponent> WidgetLocationComponent;
};
