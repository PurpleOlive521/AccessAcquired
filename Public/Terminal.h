// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "InteractableInterface.h"

#include "Terminal.generated.h"

class ACharacter;

UCLASS()
class VERTICALSLICE_API ATerminal : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	ATerminal();

	// --- Begin AActor Interface
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	// --- End AActor Interface

	// --- Begin IInteractableInterface
	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual FInteractionParams GetInteractionData_Implementation() override;
	// --- End IInteractableInterface

	UFUNCTION(BlueprintCallable)
	FVector GetIdealInteractorLocation() const;

	UFUNCTION(BlueprintCallable)
	void HealInteractor();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void DrawIdealInteractorLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void MoveInteractorToIdealLocation(AActor* InInteractor);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FInteractionParams InteractionParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<ACharacter> Interactor;

	TObjectPtr<USceneComponent> WidgetLocationComponent;

};
