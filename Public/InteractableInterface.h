// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "GameplayTagContainer.h"

#include "InteractableInterface.generated.h"



UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	EIT_Press	UMETA(DisplayName = "Press"),
	EIT_Hold	UMETA(DisplayName = "Hold"),
};

USTRUCT(BlueprintType)
struct FInteractionRequirements
{
	GENERATED_BODY()

	// The tags that the interactor needs to be able to interact with this object.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Player.AvailableModes"), Category = "Common")
	FGameplayTagContainer RequiredTags;

	// Only counts for successful presses
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
	bool bCanOnlyBePressedOnce = false;

	// Can this interaction be performed during combat? In most cases, no.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
	bool bAllowedDuringCombat = false;

	// Allows us to programatically specificy when something is interactable.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Common")
	bool bInteractionImpossible = false;
};

USTRUCT(BlueprintType)
struct FInteractionParams
{
	GENERATED_BODY()

	FInteractionParams() {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EInteractionType InteractionType = EInteractionType::EIT_Press;

	// The amount of time that the interact button must be held to trigger the interaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HoldDuration = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	FVector InteractWidgetLocation = FVector{};

	UPROPERTY(BlueprintReadWrite)
	bool bPreviouslyInteractedWith = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInteractionRequirements InteractionRequirements = {};
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VERTICALSLICE_API IInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	void Interact(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactable")
	FInteractionParams GetInteractionData();
};
