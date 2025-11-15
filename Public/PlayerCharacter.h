// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "ControllableCharacterInterface.h"
#include "CharacterBase.h"

#include "PlayerCharacter.generated.h"

class UAttackData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackHitSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPerfectDodgeEffectEndSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModeSwitchedSignature, EDamageType, NewMode);

UENUM(BlueprintType)
enum class EBufferableAction : uint8
{
	BA_None			UMETA(DisplayName = "None"),
	BA_Attack		UMETA(DisplayName = "Attack"),
	BA_HeavyAttack	UMETA(DisplayName = "Heavy Attack"),
	BA_Dash			UMETA(DisplayName = "Dash"),
	BA_ModeSwitch	UMETA(DisplayName = "Mode Switch"),
};

// Different types of Energy modifying actions. Keeps the values for each type of action centralised.
UENUM(BlueprintType)
enum class EEnergyModifyingAction : uint8
{
	EMA_AttackHit			UMETA(DisplayName = "AttackHit"), 		// When an attack successfully hit an Enemy
	EMA_PerfectDodge		UMETA(DisplayName = "PerfectDodge"), 	// When the player performs a perfect dodge
	EMA_AttackHitWeakness	UMETA(DisplayName = "AttackHitWeakness"),
	EMA_HitByEnemy			UMETA(DisplayName = "HitByEnemy"), 		// When player gets hit by enemy

	EMA_IdleTick			UMETA(DisplayName = "IdleTick"), 		// Called each tick when being idle for too long
	EMA_None				UMETA(DisplayName = "Invalid Enum"), 	// For Enum max and possible override value
};

UCLASS()
class VERTICALSLICE_API APlayerCharacter : public ACharacterBase, public IControllableCharacterInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	// --- Begin AActor Interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	// --- End AActor Interface


	// --- Begin ICombatSystemInterface
	FAAResolvedDamageEvent ReceiveDamageEvent_Implementation(FAADamageEvent DamageData) override;

	virtual bool IsDead_Implementation() override;

	virtual bool ReserveAttackToken_Implementation(int Amount) override;

	virtual void ReturnAttackToken_Implementation(int Amount) override;
	// --- End ICombatSystemInterface

	// --- Damage Events

	// Processes a Damage Event and handles it according to the state of the player.
	FAAResolvedDamageEvent ResolveDamageEvent(const FAADamageEvent& DamageEvent);

	// Blueprints opportunity to react to damage being taken.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Damage Event Received"), Category = "DamageEvents")
	void K2_OnDamageEventResolved(const FAAResolvedDamageEvent& ResolvedEvent);

	// --- Energy Management

	// Request to use a certain amount of Energy. Removes the requested amount and returns success if enough Energy is held.
	UFUNCTION(BlueprintCallable, Category = "Energy")
	bool RequestEnergyUse(float EnergyToUse);

	// Modifies the current Energy by the specified Delta amount. Use this over directly accessing the Energy attribute.
	UFUNCTION(BlueprintCallable, Category = "Energy")
	void ModifyEnergy(float Delta, bool bInterruptIdleness = true);

	// Modifies the current Energy based on a predefined action. Use this over directly accessing the Energy attribute.
	UFUNCTION(BlueprintCallable, Category = "Energy")
	void ModifyEnergyByAction(EEnergyModifyingAction Action);

	// --- Mode Switching

	// Sets the mode and attempts to activate the corresponding Mode Switch Ability for VFX activation and animations.
	UFUNCTION(BlueprintCallable, Category = "ModeSwitching")
	void SetMode(EDamageType InMode);

	// Changes the mode, calls delegates and drives the backend logic.
	UFUNCTION(BlueprintCallable, Category = "ModeSwitching")
	void SetModeInternal(EDamageType InMode);

	// Gives Blueprint an opportunity to act on a new mode being set.
	UFUNCTION(BlueprintImplementableEvent, Category = "ModeSwitching", meta = (DisplayName = "On Mode Set"))
	void K2_SetModeInternal();

	// Sets the mode back to it's default value without the use of abilities or animations.
	UFUNCTION(BlueprintCallable, Category = "ModeSwitching")
	void ResetMode();

	// Gives Blueprint an opportunity to act when the mode is reset.
	UFUNCTION(BlueprintImplementableEvent, Category = "ModeSwitching", meta = (DisplayName = "On Mode Reset"))
	void K2_ResetMode();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "ModeSwitching")
	EDamageType GetMode() const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "ModeSwitching")
	bool HasMode(EDamageType Mode);

	// Returns a percentage value, between 0 and 1.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ModeSwitching")
	float GetModeDurationRemaining() const;


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnDeath();

	UFUNCTION(BlueprintCallable, Category = "ModeSwitching")
	bool IsPlayerInCombat() const;

	// --- Idleness

	// Blueprint event for checking and responding to idleness in the player. Allows for overrides or conditional logic to pause or prevent feedback.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Idleness")
	void HandleIdleness();

	// Called when an action is performed that would interrupt "idleness", and handles the response.
	UFUNCTION(BlueprintCallable, Category = "Idleness")
	void InterruptIdleness();

	UFUNCTION(BlueprintCallable, Category = "Idleness")
	void SetToIdle();

	// --- Etc.

	// Hook-in to allow us to respond when a perfect dodge is performed by the player
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Combat")
	void OnPerfectDodge();



	// Assigns the DamageEvent that is used to resolve the next damage event the player causes.
	UFUNCTION(BlueprintCallable, Category = "DamageEvents")
	void AssignDamageEvent(const FAADamageEvent& DamageEvent);

	// Returns the FAADamageEvent that is in use by the current attack. Remember that this is done by value and not reference!
	UFUNCTION(BlueprintCallable, Category = "DamageEvents")
	FAADamageEvent GetDamageEvent() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void IncrementAttackCounter();

	// Utility

	// Does the necessary conversion to map the input to a 2D plane, relative to the camera. Maps left-right input to camera-x and up-down to camera-y.
	UFUNCTION(BlueprintCallable, Category = "Utility")
	FVector GetCameraAdjustedMovementInput() const;

	// Gets the direction that the player last tried to move in, in relation to the camera. A right-input will return the direction to the camera's right, rather than the character's right.
	// If the latest input is near zero, forward is chosen instead.
	UFUNCTION(BlueprintCallable, Category = "Utility")
	FVector GetDesiredMovementDirection() const;


protected:

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	EBufferableAction StoredBufferableAction = EBufferableAction::BA_None;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsPerformingAction = false;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsBufferingAction = false;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsPlayerDead = false;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsInCombat = false;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TArray<TSubclassOf<UGameplayAbility>> AttackAbilities;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	int AttackCounter = 0;


	// All enemies that are agroed on the player and are trying to attack them.
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TSet<TObjectPtr<AActor>> HostileActors;

	UPROPERTY(BlueprintReadWrite, Category = "ModeSwitching")
	EDamageType CurrentMode;

	UPROPERTY(BlueprintReadWrite, Category = "ModeSwitching")
	TMap<EDamageType, TSubclassOf<UGameplayAbility>> ModeSwitchAbilities;

	// --- Idleness

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Idleness")
	float MaxIdleTime = 2.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Idleness")
	bool bIsIdle = true;
	// --- Etc

	UPROPERTY(BlueprintReadWrite)
	bool bIsPerfectDodgeEffectActive = false;

	UPROPERTY(BlueprintReadWrite)
	float PerfectDodgeTimeSlow = 0.25f;

	UPROPERTY(BlueprintReadWrite)
	float PerfectDodgeDuration = 0.80f;

	UPROPERTY(BlueprintReadWrite)
	FVector2D LatestMovementInput = {};


private:
	FAADamageEvent CurrentDamageEvent;

	FTimerHandle ModeSwitchDurationHandle;

	FTimerHandle IdlenessCountdownHandle;

public:
	// --- Delegates
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnAttackHitSignature OnAttackHitDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnModeSwitchedSignature OnModeSwitchedDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable, BlueprintReadWrite)
	FOnPerfectDodgeEffectEndSignature OnPerfectDodgeEffectEndDelegate;
};
