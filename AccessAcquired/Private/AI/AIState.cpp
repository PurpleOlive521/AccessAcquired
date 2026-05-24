// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AI/AIState.h"
#include "EnemyAIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameplaySystemComponent.h"
#include "DevCommons.h"
#include "GameplayTagDefines.h"

void UAIStateLayer::Init(AEnemyAIControllerBase* InController)
{
	Controller = InController;
}

bool UAIStateLayer::CanBecomePrimary(AEnemyAIControllerBase* InjectedController) const
{
	return true;
}

void UAIStateLayer::OnBecomePrimary()
{
	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();

	Blackboard->SetValueAsEnum(Controller->StateKeyName, (uint8)State);
}

void UAIStateLayer::OnRemovedFromPrimary()
{

}

void UAIStateLayer::OnAdded()
{

}

void UAIStateLayer::OnRemoved()
{

}

UAIStateLayer_Passive::UAIStateLayer_Passive()
{
	State = EAIState::AS_Passive;
}

bool UAIStateLayer_Passive::CanBecomePrimary(AEnemyAIControllerBase* InjectedController) const
{
	const bool bSuccess = Super::CanBecomePrimary(InjectedController);
	if (!bSuccess)
	{
		return false;
	}

	return true;
}

void UAIStateLayer_Passive::OnBecomePrimary()
{
	Super::OnBecomePrimary();
}

UAIStateLayer_Attacking::UAIStateLayer_Attacking()
{
	State = EAIState::AS_Attacking;
}

bool UAIStateLayer_Attacking::CanBecomePrimary(AEnemyAIControllerBase* InjectedController) const
{
	const bool bSuccess = Super::CanBecomePrimary(InjectedController);
	if (!bSuccess)
	{
		return false;
	}

	AActor* DerefTarget = Target.Get();
	if (DerefTarget)
	{
		const bool bRegistered = InjectedController->RegisterTarget(FAIStimulus(), DerefTarget);

		return bRegistered;
	}

	AActor* ExistingTarget = InjectedController->GetTarget();
	if (!ExistingTarget)
	{
		return false;
	}

	return true;
}

void UAIStateLayer_Attacking::OnBecomePrimary()
{
	Super::OnBecomePrimary();

	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	AActor* ExistingTarget = Controller->GetTarget();
	check(ExistingTarget);

	Blackboard->SetValueAsObject(Controller->CombatTargetKeyName, ExistingTarget);
}

void UAIStateLayer_Attacking::OnAdded()
{
	APawn* OwnerPawn = Controller->GetPawn();

	UGameplaySystemComponent* Component = UGameplaySystemComponent::GetGameplaySystemFromActor(OwnerPawn);
	if (Component)
	{
		Component->AddTag(GAMEPLAYTAG_Status_CombatReady_InCombat);
	}
}

void UAIStateLayer_Attacking::OnRemoved()
{
	APawn* OwnerPawn = Controller->GetPawn();

	UGameplaySystemComponent* Component = UGameplaySystemComponent::GetGameplaySystemFromActor(OwnerPawn);
	if (Component)
	{
		Component->RemoveTag(GAMEPLAYTAG_Status_CombatReady_InCombat);
	}
}

UAIStateLayer_Frozen::UAIStateLayer_Frozen()
{
	State = EAIState::AS_Frozen;
}

bool UAIStateLayer_Frozen::CanBecomePrimary(AEnemyAIControllerBase* InjectedController) const
{
	return true;
}

void UAIStateLayer_Frozen::OnBecomePrimary()
{
	Super::OnBecomePrimary();
}

void UAIStateLayer_Frozen::OnRemovedFromPrimary()
{
	Super::OnRemovedFromPrimary();

	const EAIState CurrentState = Controller->GetCurrentState();

	if (CurrentState == EAIState::AS_Passive)
	{
		Controller->PushState<UAIStateLayer_Attacking>();
	}
}

UAIStateLayer_Dead::UAIStateLayer_Dead()
{
	State = EAIState::AS_Dead;
}

bool UAIStateLayer_Dead::CanBecomePrimary(AEnemyAIControllerBase* InjectedController) const
{
	return true;
}

void UAIStateLayer_Dead::OnBecomePrimary()
{
	Super::OnBecomePrimary();

	Controller->ForgetAllTargets();
}

UAIStateLayer* UAIStateLayerFactory::CreateLayer(TSubclassOf<UAIStateLayer> Class, AEnemyAIControllerBase* Controller)
{
	check(Controller);

	UAIStateLayer* Layer = NewObject<UAIStateLayer>(Controller, Class);
	Layer->Init(Controller);

	return Layer;
}

UAIStateLayer_Passive* UAIStateLayerFactory::CreateLayer_Passive(AEnemyAIControllerBase* Controller)
{
	check(Controller);

	UAIStateLayer_Passive* Layer = NewObject<UAIStateLayer_Passive>(Controller, UAIStateLayer_Passive::StaticClass());
	Layer->Init(Controller);

	return Layer;
}

UAIStateLayer_Attacking* UAIStateLayerFactory::CreateLayer_Attacking(AEnemyAIControllerBase* Controller, AActor* Target)
{
	check(Controller);

	UAIStateLayer_Attacking* Layer = NewObject<UAIStateLayer_Attacking>(Controller, UAIStateLayer_Attacking::StaticClass());
	Layer->Init(Controller);

	Layer->Target = Target;

	return Layer;
}

UAIStateLayer_Frozen* UAIStateLayerFactory::CreateLayer_Frozen(AEnemyAIControllerBase* Controller)
{
	check(Controller);

	UAIStateLayer_Frozen* Layer = NewObject<UAIStateLayer_Frozen>(Controller, UAIStateLayer_Frozen::StaticClass());
	Layer->Init(Controller);

	return Layer;
}

UAIStateLayer_Dead* UAIStateLayerFactory::CreateLayer_Dead(AEnemyAIControllerBase* Controller)
{
	check(Controller);

	UAIStateLayer_Dead* Layer = NewObject<UAIStateLayer_Dead>(Controller, UAIStateLayer_Dead::StaticClass());
	Layer->Init(Controller);

	return Layer;
}
