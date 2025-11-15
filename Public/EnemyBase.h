// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "CharacterBase.h"
#include "ChargeComponent.h"
#include "SaveableObjectInterface.h"

#include "EnemyBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeadSignature, AActor*, Enemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, bool, IsCalledOnLoad);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMeleeAttackSignature);

UENUM(BlueprintType)
enum class EAiStates : uint8
{
	AS_Passive			UMETA(DisplayName = "Passive"),
	AS_Attacking		UMETA(DisplayName = "Hostile"),
	AS_Frozen			UMETA(DisplayName = "Frozen"), // Stunned, in hit animation, pressured, etc.
	AS_Dead				UMETA(DisplayName = "Dead"),
};

UENUM(BlueprintType)
enum class EAiSense : uint8
{
	AS_None				UMETA(DisplayName = "None"),
	AS_Sight			UMETA(DisplayName = "Sight"),
	AS_Damage			UMETA(DisplayName = "Damage"),
};

UCLASS()
class VERTICALSLICE_API AEnemyBase : public ACharacterBase, public ISaveableObjectInterface
{
	GENERATED_BODY()

public:
	AEnemyBase();

	// --- Begin AActor Interface
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// --- End AActor Interface

	// --- Begin ICombatSystemInterface
	FAAResolvedDamageEvent ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent) override;

	virtual bool IsDead_Implementation() override;
	// --- End ICombatSystemInterface

	// --- Begin ISaveableObjectInterface
	virtual void SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject) override;

	virtual void LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject) override;
	// --- End ISaveableObjectInterface

	// Ensures that the ID is not copied over when the Actor is duplicated in the Editor.
	virtual void PostEditImport() override;

	// Called when a damage event was received that was not resolved as ignored (e.g bWasEventIgnored = true). Gives blueprints an opportunity to act to damage being taken.
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamageTaken(const FAAResolvedDamageEvent& ResolvedEvent);

	UChargeComponent* GetPressureComponent();

	// Checks if the enemies health has dropped below 0
	UFUNCTION()
	void OnHealthChanged(EAttributeType Attribute);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool SwitchModeNative(EDamageType TargetMode);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnEnemyKilled(bool bIsOnLoad);

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	EDamageType CurrentMode = EDamageType::EDT_Normal;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsEnemyDead = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGuidTag Id;

private:

	TObjectPtr<UChargeComponent> PressureComponent;

public:	
	// --- Delegates

	// Meant for alerting other entities of this enemies death. Called after all death-handling logic is performed
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnEnemyDeadSignature OnEnemyDeadDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnMeleeAttackSignature OnMeleeAttackDelegate;

	// Meant for internal communication within the enemy systems
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDeathSignature OnDeathDelegate;
};
