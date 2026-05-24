// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CombatTypes.h"

#include "ControllableCharacterInterface.generated.h"

UENUM(BlueprintType)
enum class EDetectionSource : uint8
{
	EDT_Enemy				UMETA(DisplayName = "Enemy"),
	EDT_Camera				UMETA(DisplayName = "Camera"),
	EDT_SurveillanceDrone	UMETA(DisplayName = "SurveillanceDrone"),

	EDT_None				UMETA(DisplayName = "None")
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UControllableCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Allows input to be forwarded to a ambigouous pawn, where the pawn can customize its response to some or all events.
 */
class VERTICALSLICE_API IControllableCharacterInterface
{
	GENERATED_BODY()

public:

	// --- Input Forwarding Events

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnMove(float ActionValueX, float ActionValueY);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnLook(float ActionValueX, float ActionValueY);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnJump();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnAttack();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnHeavyAttack();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnLockTarget();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnDash();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnSprint();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnModeSwitch(EDamageType Mode);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnInteract(bool bIsOngoing, float ElapsedSeconds, bool bIsCancelledOrCompleted);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnAbility();

	// If bCompleted is false, interpret call as on Started
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnModeSwitchToggle(bool bCompleted);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	float GetInputMovementDirection();


	// DetectionSource allows us to handle the event dynamically, since an Enemy detecting and aggroing on the player 
	// toggles combat while cameras and drones teleport us.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void DetectedByEntity(AActor* Entity, EDetectionSource DetectionSource);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void EntityLostInterest(AActor* Entity, EDetectionSource DetectionSource);
};
