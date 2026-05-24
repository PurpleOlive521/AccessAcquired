// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AAAITypes.h"
#include "AIState.h"
#include "EnemyAIControllerBase.generated.h"

class AEnemyBase;
class UAIPerceptionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateRemovedSignature, UAIStateLayer*, RemovedState);

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API AEnemyAIControllerBase : public AAIController
{
	GENERATED_BODY()

	friend class UAAExtendedDebugWidget;

public:
	AEnemyAIControllerBase(const FObjectInitializer& ObjectInitializer);

	// --- Begin AActor Interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// --- End AActor Interface

	// --- Begin AAIController Interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	// --- End AAIController Interface

	UFUNCTION(BlueprintImplementableEvent)
	UAIPerceptionComponent* GetMainPerceptionComponent() const;

	UFUNCTION(BlueprintImplementableEvent)
	UAIPerceptionComponent* GetProximityPerceptionComponent() const;

	// --- State

	// Pushes the layer onto the top of the stack, becoming Primary. Returns the element for easier method chaining.
	UFUNCTION(BlueprintCallable)
	UAIStateLayer* PushState(UAIStateLayer* Layer);

	// Templated version of above. 
	template<class T>
	T* PushState()
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UAIStateLayer>::Value, "'T' template parameter to PushState must be derived from UAIStateLayer");

		T* Layer = UAIStateLayerFactory::CreateLayer<T>(this);
		return (T*)PushState(Layer);
	}

	// Returns the top layer. The new top becomes Primary.
	UFUNCTION(BlueprintCallable)
	UAIStateLayer* PopState();

	UFUNCTION(BlueprintCallable)
	void RemoveState(UAIStateLayer* StateLayer);

	// Returns the top layer.
	UAIStateLayer* GetPrimaryState();

	template<class T>
	T* GetPrimaryState() const
	{
		return Cast<T>(GetPrimaryState());
	}

	// Returns the first State of type T found, searching the StateStack from top to bottom.
	UFUNCTION(BlueprintCallable)
	UAIStateLayer* GetStateByClass(TSubclassOf<UAIStateLayer> Class) const;

	// Templated version of above. 
	template<class T>
	T* GetStateByClass() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UAIStateLayer>::Value, "'T' template parameter to GetFirstStateOfType must be derived from UAIStateLayer");

		return (T*)GetStateByClass(T::StaticClass());
	}

	// Returns the current fallback or creates a new one. Guaranteed to return a valid UAIStateLayer during runtime.
	UAIStateLayer* GetFallbackState();

	// Recursively tries to find a layer to seat as the Primary in the StateStack.
	void SeatPrimary();
	
	// Removes the target from our list of candidates.
	// Returns false if the Actor was not found.
	UFUNCTION(BlueprintCallable)
	bool ForgetTarget(AActor* Target);

	// Removes all targets from our list of candidates.
	// Returns the amount of targets forgotten.
	UFUNCTION(BlueprintCallable)
	int ForgetAllTargets();

	// Adds the target to our list of candidates.
	// Returns true even if the target is already registered.
	UFUNCTION(BlueprintCallable)
	bool RegisterTarget(const FAIStimulus& Stimulus, AActor* Target);

	// Returns the highest priority target known to this Controller. Can return nullptr if there are no known targets.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetTarget() const;

	// Returns our current target. Is not necessarily the same known target as GetTarget.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetCurrentTarget() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	[[nodiscard]] EAIState GetCurrentState();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AIController|Keys")
	FName StateKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AIController|Keys")
	FName CombatTargetKeyName;
	
protected:

	UFUNCTION()
	void OnPerceptionUpdated_Main(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnPerceptionUpdated_Proximity(AActor* Actor, FAIStimulus Stimulus);

	[[nodiscard]] EAISense GetSenseFromStimulus(const FAIStimulus& Stimulus);

	void HandleSensedSight(const FAIStimulus& Stimulus, AActor* Actor);

	void HandleSensedDamage(const FAIStimulus& Stimulus, AActor* Actor);

	void HandleSensedProximity(const FAIStimulus& Stimulus, AActor* Actor);

	void HandleSensedHearing(const FAIStimulus& Stimulus, AActor* Actor);

	UPROPERTY(BlueprintReadOnly, Category = "AIController")
	TObjectPtr<AEnemyBase> ControlledEnemy = nullptr;

	// Only used if the controlled Pawn does not have a valid BehaviourTree.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AIController")
	TObjectPtr<UBehaviorTree> DefaultBehaviorTree = nullptr;

	TObjectPtr<UAIPerceptionComponent> MainPerceptionComponent = nullptr;

	TObjectPtr<UAIPerceptionComponent> ProximityPerceptionComponent = nullptr;

	// All potential targets known to this AIController.
	TSet<FKnownTargetData> KnownTargets;

	UPROPERTY()
	TArray<TObjectPtr<UAIStateLayer>> StateStack;

	TObjectPtr<UAIStateLayer> Primary = nullptr;

	UPROPERTY()
	TObjectPtr<UAIStateLayer> FallbackLayer = nullptr;

public:
	// --- Delegates
	UPROPERTY(BlueprintAssignable)
	FOnStateRemovedSignature OnStateRemovedDelegate;
};
