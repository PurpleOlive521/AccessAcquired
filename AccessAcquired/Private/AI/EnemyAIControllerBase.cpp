// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "EnemyAIControllerBase.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/CrowdManager.h"
#include "EnemyBase.h"
#include "DevCommons.h"
#include "ControllableCharacterInterface.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "ReceiveDamageEventInterface.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "AISense_AADamage.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "AIState.h"

const TMap<TSubclassOf<UAISense>, EAISense> AiSenseMap = {
															{UAISense_AADamage::StaticClass(),	EAISense::AS_Damage},
															{UAISense_Sight::StaticClass(),		EAISense::AS_Sight},
															{UAISense_Hearing::StaticClass(),	EAISense::AS_Hearing},
														 };

AEnemyAIControllerBase::AEnemyAIControllerBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	StateKeyName = FName("State");
	CombatTargetKeyName = FName("AttackTarget");
}

void AEnemyAIControllerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// We need to unregister manually
	UCrowdManager* CrowdManager = UCrowdManager::GetCurrent(this);
	if (CrowdManager)
	{
		UCrowdFollowingComponent* CrowdFollowingComp = GetComponentByClass<UCrowdFollowingComponent>();

		if (CrowdFollowingComp)
		{
			CrowdManager->UnregisterAgent(CrowdFollowingComp);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AEnemyAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledEnemy = Cast<AEnemyBase>(InPawn);

	if (!ControlledEnemy)
	{
		UE_LOG(VSLog, Error, TEXT("AEnemyAIControllerBase: Possessed Pawn could not be cast to AEnemyBase!"));
		return;
	}

	SeatPrimary();

	UBehaviorTree* UsedBehaviourTree = DefaultBehaviorTree;
	if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(InPawn))
	{
		if (EnemyBase->BehaviourTree)
		{
			UsedBehaviourTree = EnemyBase->BehaviourTree;
		}
	}

	const bool bSuccess = RunBehaviorTree(UsedBehaviourTree);

	if (!bSuccess)
	{
		UE_LOG(VSLog, Error, TEXT("AEnemyAIControllerBase: Could not run Behavior Tree! Is the asset null?"));
	}

	MainPerceptionComponent = GetMainPerceptionComponent();
	if (MainPerceptionComponent)
	{
		MainPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AEnemyAIControllerBase::OnPerceptionUpdated_Main);
	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("AEnemyAIControllerBase: Missing main UAIPerceptionComponent, won't respond to stimulus."));
	}

	ProximityPerceptionComponent = GetProximityPerceptionComponent();
	if (ProximityPerceptionComponent)
	{
		ProximityPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AEnemyAIControllerBase::OnPerceptionUpdated_Proximity);
	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("AEnemyAIControllerBase: Missing proximity UAIPerceptionComponent, won't respond to stimulus."));
	}
}

void AEnemyAIControllerBase::OnUnPossess()
{
	Super::OnUnPossess();

	if (MainPerceptionComponent)
	{
		MainPerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AEnemyAIControllerBase::OnPerceptionUpdated_Main);
	}

	if (ProximityPerceptionComponent)
	{
		ProximityPerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AEnemyAIControllerBase::OnPerceptionUpdated_Proximity);
	}
}

UAIStateLayer* AEnemyAIControllerBase::PushState(UAIStateLayer* Layer)
{
	StateStack.Push(Layer);
	Layer->OnAdded();
	
	SeatPrimary();

	return Layer;
}

UAIStateLayer* AEnemyAIControllerBase::PopState()
{
	if (!StateStack.Num())
	{
		return nullptr;
	}

	UAIStateLayer* PoppedLayer = StateStack.Pop();
	PoppedLayer->OnRemoved();

	SeatPrimary();

	OnStateRemovedDelegate.Broadcast(PoppedLayer);
	return PoppedLayer;
}

void AEnemyAIControllerBase::RemoveState(UAIStateLayer* StateLayer)
{
	const int32 RemovedCount = StateStack.Remove(StateLayer);

	ensure(RemovedCount <= 1);

	if (RemovedCount)
	{
		StateLayer->OnRemoved();
		SeatPrimary();
		OnStateRemovedDelegate.Broadcast(StateLayer);
	}
}

UAIStateLayer* AEnemyAIControllerBase::GetPrimaryState()
{
	return Primary;
}

UAIStateLayer* AEnemyAIControllerBase::GetStateByClass(TSubclassOf<UAIStateLayer> Class) const
{
	check(Class);

	EAIState Type = Class->GetDefaultObject<UAIStateLayer>()->State;

	for (int32 i = StateStack.Num() - 1; i >= 0; i--)
	{
		if (StateStack[i]->State == Type)
		{
			return StateStack[i];
		}
	}

	return nullptr;
}

UAIStateLayer* AEnemyAIControllerBase::GetFallbackState()
{
	if (FallbackLayer)
	{
		return FallbackLayer;
	}
	
	return FallbackLayer = UAIStateLayerFactory::CreateLayer_Passive(this);
}

void AEnemyAIControllerBase::SeatPrimary()
{
	if (!StateStack.Num())
	{
		UAIStateLayer* Fallback = GetFallbackState();

		// Already is Primary
		if (Fallback == Primary)
		{
			return;
		}

		const bool bSuccess = Fallback->CanBecomePrimary(this);
		if (!bSuccess)
		{
			UE_LOG(VSLog, Error, TEXT("EnemyAIControllerBase: Could not seat fallback AIStateLayer as Primary!"));
			checkNoEntry();
		}

		UAIStateLayer* PreviousPrimary = Primary;
		Primary = Fallback;

		if (PreviousPrimary)
		{
			PreviousPrimary->OnRemovedFromPrimary();
		}

		return;
	}

	const bool bSuccess = StateStack.Top()->CanBecomePrimary(this);
	if (bSuccess)
	{
		// Already is Primary
		if (StateStack.Top() == Primary)
		{
			return;
		}

		UAIStateLayer* PreviousPrimary = Primary;
		Primary = StateStack.Top();
		Primary->OnBecomePrimary();
		
		if (PreviousPrimary)
		{
			PreviousPrimary->OnRemovedFromPrimary();
		}

		return;
	}

	StateStack.Pop();
	SeatPrimary();
}

bool AEnemyAIControllerBase::ForgetTarget(AActor* Target)
{
	if (!Target)
	{
		return false;
	}

	// No further action required if not already known.
	if (!KnownTargets.Contains(Target))
	{
		return false;
	}

	IControllableCharacterInterface* Interface = Cast<IControllableCharacterInterface>(Target);
	if (Interface)
	{
		Interface->Execute_EntityLostInterest(Target, GetPawn(), EDetectionSource::EDT_Enemy);
	}
	else
	{
		UE_LOG(VSLog, Warning, TEXT("AEnemyAIControllerBase: ForgetTarget called on non-ControllableCharacter implementor. Was this intented?"));
	}

	KnownTargets.Remove(Target);
	return true;
}

int AEnemyAIControllerBase::ForgetAllTargets()
{
	int ForgottenCount = 0;

	TArray<FKnownTargetData> KnownTargetsCopy = KnownTargets.Array();
	for (const FKnownTargetData& KnownTarget : KnownTargetsCopy)
	{
		ForgottenCount += (int)ForgetTarget(KnownTarget.GetTarget());
	}

	return ForgottenCount;
}

bool AEnemyAIControllerBase::RegisterTarget(const FAIStimulus& Stimulus, AActor* Target)
{
	ensure(Target);

	// We already know about this target
	if (KnownTargets.Contains(Target))
	{
		return true;
	}

	const bool bAreSameTeam = UGameplayUtilityBlueprintLibrary::AreSameTeam(GetPawn(), Target);
	if (bAreSameTeam)
	{
		return false;
	}

	if (IReceiveDamageEventInterface* DamageInterface = Cast<IReceiveDamageEventInterface>(Target))
	{
		const bool bIsDead = DamageInterface->Execute_IsDead(Target);

		if (bIsDead)
		{
			return false;
		}
	}

	IControllableCharacterInterface* Interface = Cast<IControllableCharacterInterface>(Target);
	if (Interface)
	{
		Interface->Execute_DetectedByEntity(Target, GetPawn(), EDetectionSource::EDT_Enemy);
	}
	else
	{
		UE_LOG(VSLog, Warning, TEXT("AEnemyAIControllerBase: RegisterTarget called on non-ControllableCharacter implementor. Was this intented?"));
		return false;
	}

	KnownTargets.Add(Target);
	return true;
}

AActor* AEnemyAIControllerBase::GetTarget() const
{
	FKnownTargetData Highest = {};

	for (const FKnownTargetData& KnownTarget : KnownTargets)
	{
		if (KnownTarget.Priority >= Highest.Priority)
		{
			Highest = KnownTarget;
		}
	}

	return Highest.GetTarget();
}

AActor* AEnemyAIControllerBase::GetCurrentTarget() const
{
	UAIStateLayer_Attacking* Layer = GetStateByClass<UAIStateLayer_Attacking>();

	if (Layer)
	{
		if (AActor* Target = Layer->Target.Get())
		{
			return Target;
		}
	}

	return GetTarget();
}

EAIState AEnemyAIControllerBase::GetCurrentState()
{
	check(Primary);
	return Primary->State;
}

void AEnemyAIControllerBase::OnPerceptionUpdated_Main(AActor* Actor, FAIStimulus Stimulus)
{
	EAISense Sense = GetSenseFromStimulus(Stimulus);

	switch (Sense)
	{
	case EAISense::AS_Sight:
		HandleSensedSight(Stimulus, Actor);
		break;

	case EAISense::AS_Damage:
		HandleSensedDamage(Stimulus, Actor);
		break;

	case EAISense::AS_Proximity:
		HandleSensedProximity(Stimulus, Actor);
		break;

	case EAISense::AS_Hearing:
		HandleSensedHearing(Stimulus, Actor);
		break;
	}
}

void AEnemyAIControllerBase::OnPerceptionUpdated_Proximity(AActor* Actor, FAIStimulus Stimulus)
{
	EAISense Sense = GetSenseFromStimulus(Stimulus);

	if (Sense == EAISense::AS_Sight)
	{
		HandleSensedProximity(Stimulus, Actor);
	}
}

EAISense AEnemyAIControllerBase::GetSenseFromStimulus(const FAIStimulus& Stimulus)
{
	ensure(Stimulus.IsValid());
	
	if (!Stimulus.WasSuccessfullySensed())
	{
		return EAISense::AS_None;
	}

	const TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);

	ensure(AiSenseMap.Contains(SenseClass));

	// We assert if the type is not yet supported.
	EAISense Sense = AiSenseMap.FindChecked(SenseClass);

	return Sense;
}

void AEnemyAIControllerBase::HandleSensedSight(const FAIStimulus& Stimulus, AActor* Actor)
{
	ensure(Actor);

	RegisterTarget(Stimulus, Actor);

	const EAIState State = GetCurrentState();
	if (State == EAIState::AS_Passive)
	{
		PushState<UAIStateLayer_Attacking>();
	}
}

void AEnemyAIControllerBase::HandleSensedDamage(const FAIStimulus& Stimulus, AActor* Actor)
{
	// We currently don't treat this differently from Sight.
	HandleSensedSight(Stimulus, Actor);
}

void AEnemyAIControllerBase::HandleSensedProximity(const FAIStimulus& Stimulus, AActor* Actor)
{
	// We currently don't treat this differently from Sight.
	HandleSensedSight(Stimulus, Actor);
}

void AEnemyAIControllerBase::HandleSensedHearing(const FAIStimulus& Stimulus, AActor* Actor)
{
	// We currently don't treat this differently from Sight.
	HandleSensedSight(Stimulus, Actor);
}
