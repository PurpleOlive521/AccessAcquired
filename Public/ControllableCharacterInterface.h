// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

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

USTRUCT(BlueprintType)
struct VERTICALSLICE_API FEnemyDeathReward
{
	GENERATED_BODY()

	FEnemyDeathReward() {};
	FEnemyDeathReward(float Exp, int Intel) : Experience(Exp), Intel(Intel) {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Experience = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Intel = 0;
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
	void OnJump(bool IsFinished);

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
	void OnCycleMode();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	float GetInputMovementDirection();


	// DetectionSource allows us to handle the event dynamically, since an Enemy detecting and aggroing on the player 
	// toggles combat while cameras and drones teleport us.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void DetectedByEntity(AActor* Entity, EDetectionSource DetectionSource);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void EntityLostInterest(AActor* Entity, EDetectionSource DetectionSource);

	// Called when an entity is killed. Does not need to be caused by the player.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ControllableCharacterInterface")
	void OnEntityKilled(AActor* Entity, const FEnemyDeathReward& Reward);
};
