// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTasks/GameplayAbilityTask.h"
#include "AI/AIState.h"
#include "GAT_PushAIState.generated.h"

class AEnemyAIControllerBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAIStateRemovedSignature);

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UGAT_PushAIState : public UGameplayAbilityTask
{
	GENERATED_BODY()
	
public:

	UGAT_PushAIState(const FObjectInitializer& ObjectInitializer);

	/**
	 * Pushes a AIState to the GameplayAbilities current Owner.
	 * 
	 * @param StateClass The type of state to push. Used if OptionalState is nullptr.
	 * @param OptionalState State instance that we want to push.
	 */
	UFUNCTION(BlueprintCallable, Meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "GameplayAbility|Tasks")
	static UGAT_PushAIState* PushAIState(UGameplayAbility* OwningAbility, TSubclassOf<UAIStateLayer> StateClass, UAIStateLayer* OptionalStateInstance = nullptr);

	virtual void Activate() override;

	virtual void ExternalCancel() override;

	virtual void OnDestroy(bool AbilityEnded) override;

	virtual FString GetDebugString() const override;

	void RemoveState();

	UFUNCTION()
	void OnStateRemoved(UAIStateLayer* RemovedState);

protected:

	TSubclassOf<UAIStateLayer> StateClass;

	TObjectPtr<UAIStateLayer> StateInstance = nullptr;

	TWeakObjectPtr<AEnemyAIControllerBase> BoundAIController = nullptr;

public:
	// --- Delegates

	UPROPERTY(BlueprintAssignable)
	FOnAIStateRemovedSignature OnAIStateRemovedDelegate;
};
