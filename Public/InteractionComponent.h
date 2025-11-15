// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "InteractableInterface.h"

#include "InteractionComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VERTICALSLICE_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetActiveInteractable(TArray<AActor*> Interactables);

	// Checks if the player can currently use the Interactable. 
	// Returns true if the interaction is possible, false otherwise.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool CheckInteractionRequirements(FInteractionRequirements Requirements, bool bPressedPreviously);
};
