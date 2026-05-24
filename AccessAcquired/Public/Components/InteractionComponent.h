// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableInterface.h"
#include "Components/WidgetComponent.h"
#include "WC_InteractionPrompt.h"
#include "InteractionComponent.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VERTICALSLICE_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Checks if the player can currently use the Interactable. 
	// Returns true if the interaction is possible, false otherwise.
	UFUNCTION(BlueprintCallable, Category = "InteractionComponent")
	bool CheckInteractionRequirements(const FInteractionParams& Params);

	UFUNCTION(BlueprintCallable, Category = "InteractionComponent")
	void InteractWithFocusInteractable();

	UFUNCTION()
	void OnReceiveBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnReceiveEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "InteractionComponent")
	void AssignFocusInteractable(AActor* Interactable);

	UFUNCTION(BlueprintCallable, Category = "InteractionComponent")
	void KeepWidgetOnInteractable(AActor* Interactable);

protected:

	void CreateInteractionWidget();

	void ProcessInteractables();

	UPROPERTY(EditDefaultsOnly, Category = "InteractionComponent")
	TSubclassOf<UWC_InteractionPrompt> InteractionWidgetClass = UWidgetComponent::StaticClass();

	UPROPERTY()
	TObjectPtr<UWC_InteractionPrompt> InteractionWidgetInstance = nullptr;

	TArray<TObjectPtr<AActor>> AvailableInteractables;

	TObjectPtr<AActor> FocusInteractable = nullptr;
};
