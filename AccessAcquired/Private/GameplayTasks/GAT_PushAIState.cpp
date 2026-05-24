// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayTasks/GAT_PushAIState.h"
#include "EnemyAIControllerBase.h"

UGAT_PushAIState::UGAT_PushAIState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UGAT_PushAIState* UGAT_PushAIState::PushAIState(UGameplayAbility* OwningAbility, TSubclassOf<UAIStateLayer> StateClass, UAIStateLayer* OptionalStateInstance)
{
	UGAT_PushAIState* NewTask = NewAbilityTask<UGAT_PushAIState>(OwningAbility);

	NewTask->StateClass = StateClass;
	NewTask->StateInstance = OptionalStateInstance;

	return NewTask;
}

void UGAT_PushAIState::Activate()
{
	check(Ability);

	if (APawn* Pawn = Cast<APawn>(Ability->GetOwningActor()))
	{
		if (AEnemyAIControllerBase* AIController = Cast<AEnemyAIControllerBase>(Pawn->GetController()))
		{
			if (!StateInstance)
			{
				StateInstance = UAIStateLayerFactory::CreateLayer(StateClass, AIController);
			}
			else
			{
				// For DebugString
				StateClass = StateInstance->GetClass();
			}

			AIController->PushState(StateInstance);

			AIController->OnStateRemovedDelegate.AddUniqueDynamic(this, &UGAT_PushAIState::OnStateRemoved);

			BoundAIController = MakeWeakObjectPtr(AIController);
		}
	}
}

void UGAT_PushAIState::ExternalCancel()
{
	RemoveState();

	Super::ExternalCancel();
}

void UGAT_PushAIState::OnDestroy(bool AbilityEnded)
{
	RemoveState();

	Super::OnDestroy(AbilityEnded);
}

FString UGAT_PushAIState::GetDebugString() const
{
	return FString::Printf( TEXT("PushAIState: %s"), *StateClass->GetName());
}

void UGAT_PushAIState::RemoveState()
{
	if (AEnemyAIControllerBase* AIController = BoundAIController.Get())
	{
		AIController->OnStateRemovedDelegate.RemoveAll(this);

		AIController->RemoveState(StateInstance);
	}
}

void UGAT_PushAIState::OnStateRemoved(UAIStateLayer* RemovedState)
{
	if (RemovedState == StateInstance)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnAIStateRemovedDelegate.Broadcast();
		}
	}
}
