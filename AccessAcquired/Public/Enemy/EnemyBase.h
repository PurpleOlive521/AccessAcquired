// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "CharacterBase.h"
#include "ChargeComponent.h"
#include "SaveableObjectInterface.h"
#include "TargetLockableComponent.h"

#include "EnemyBase.generated.h"

class AEnemyBase;
class UBehaviorTree;

constexpr float DAMAGE_TAKEN_LOUDNESS = 1.0f;
constexpr float DAMAGE_TAKEN_MAX_RANGE = 500.0f;

constexpr int	INTEL_GAINED_PER_LEVEL = 100;
constexpr float INTEL_RANDOMIZE_MIN = 0.8f;
constexpr float INTEL_RANDOMIZE_MAX = 1.2f;

constexpr float EXPERIENCE_GAINED_PER_LEVEL = 80.0f;
constexpr float EXPERIENCE_RANDOMIZE_MIN = 0.95f;
constexpr float EXPERIENCE_RANDOMIZE_MAX = 1.05f;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDeadSignature, AEnemyBase*, Enemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMeleeAttackSignature);

UCLASS()
class VERTICALSLICE_API AEnemyBase : public ACharacterBase, public ISaveableObjectInterface
{
	GENERATED_BODY()

public:
	AEnemyBase(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	// --- Begin ICombatSystemInterface
	FAAResolvedDamageEvent ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent) override;
	// --- End ICombatSystemInterface

	// --- Begin SaveableObject Interface
	bool ShouldAlwaysTrackDestroyed_Implementation();

	virtual bool OnSerialize(FSaveGameArchive& Archive, bool bIsLoading) override;
	// --- End SaveableObject Interface

	virtual void OnDying() override;

	virtual void OnDead(bool bIsOnLoad) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Get AI Controller"))
	AEnemyAIControllerBase* GetAIController();

	// Called when a damage event was received that was not resolved as ignored (e.g bWasEventIgnored = true). Gives blueprints an opportunity to act to damage being taken.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Damage Taken"))
	void K2_OnDamageTaken(const FAAResolvedDamageEvent& ResolvedEvent);

	UChargeComponent* GetChargeComponent();

	void SetSpawnpoint(AEnemySpawnpoint* InSpawnpoint);

	UFUNCTION(BlueprintCallable)
	AEnemySpawnpoint* GetSpawnpoint();

	UFUNCTION()
	void ListenForGameplayTags(FGameplayTag ModifiedTag, bool bWasAdded);

	void TriggerCombatStarted();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Enter Combat"), Category = "Combat")
	void K2_OnEnterCombat();

	void TriggerCombatEnded();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Exit Combat"), Category = "Combat")
	void K2_OnExitCombat();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviourTree = nullptr;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UChargeComponent> ChargeComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTargetLockableComponent> TargetLockableComponent = nullptr;

	UPROPERTY(SaveGame)
	TWeakObjectPtr<AEnemySpawnpoint> Spawnpoint = nullptr;

	TObjectPtr<AEnemyAIControllerBase> AIController = nullptr;

public:	
	// --- Delegates

	// Called after all death-handling logic is performed, but before destroying the actor.
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnEnemyDeadSignature OnEnemyDeadDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnMeleeAttackSignature OnMeleeAttackDelegate;
};
