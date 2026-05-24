// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "ControllableCharacterInterface.h"
#include "CharacterBase.h"

#include "PlayerCharacter.generated.h"

class UAttackData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackHitSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPerfectDodgeEffectEndSignature);

constexpr int HIGH_ENEMY_COUNT = 6;
constexpr int MEDIUM_ENEMY_COUNT = 4;

constexpr int HIGH_ENEMY_ATTACK_TOKENS = 3;
constexpr int MEDIUM_ENEMY_ATTACK_TOKENS = 2;
constexpr int LOW_ENEMY_ATTACK_TOKENS = 1;

UCLASS()
class VERTICALSLICE_API APlayerCharacter : public ACharacterBase, public IControllableCharacterInterface, public ISaveableObjectInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	// --- Begin AActor Interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// --- End AActor Interface

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITORONLY_DATA

	// --- Begin ICombatSystemInterface
	FAAResolvedDamageEvent ReceiveDamageEvent_Implementation(FAADamageEvent DamageData) override;

	virtual bool ReserveActionTokens_Implementation(int Amount) override;

	virtual void ReturnActionTokens_Implementation(int Amount) override;
	// --- End ICombatSystemInterface

	// --- Begin SaveableObject Interface
	virtual bool OnSerialize(FSaveGameArchive& Archive, bool bIsLoading) override;

	const FGuid GetSpawnID_Implementation() const override;

	bool SetSpawnID_Implementation(const FGuid& NewID) override;

	bool IsGlobalData_Implementation() override;
	// --- Begin SaveableObject Interface

	// --- Damage Events

	// Processes a Damage Event and handles it according to the state of the player.
	FAAResolvedDamageEvent ResolveDamageEvent(const FAADamageEvent& DamageEvent);

	// Blueprints opportunity to react to damage being taken.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Damage Event Received"), Category = "DamageEvents")
	void K2_OnDamageEventResolved(const FAAResolvedDamageEvent& ResolvedEvent);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Enter Combat"), Category = "Combat")
	void K2_OnEnterCombat();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Exit Combat"), Category = "Combat")
	void K2_OnExitCombat();

	// Clamps the value before returning it.
	UFUNCTION(BlueprintCallable)
	int32 GetDefaultComboCounter();

	// Clamps the value before returning it.
	UFUNCTION(BlueprintCallable)
	int32 GetHeavyComboCounter();

	UFUNCTION()
	void ListenForGameplayTags(FGameplayTag ModifiedTag, bool bWasAdded);

	void TriggerCombatStarted();

	void TriggerCombatEnded();

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TArray<TSubclassOf<UGameplayAbility>> AttackAbilities;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TArray<TSubclassOf<UGameplayAbility>> HeavyAttackAbilities;

	UPROPERTY(BlueprintReadWrite)
	FVector2D LatestMovementInput = {};

	UPROPERTY(BlueprintReadOnly)
	FGuid SpawnId;

	// TODO: Make a more dedicated pipeline for this information.
	UPROPERTY(BlueprintReadWrite)
	AActor* DetectionInstigator = nullptr;
};
