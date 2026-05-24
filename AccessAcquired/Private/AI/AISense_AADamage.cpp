// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AISense_AADamage.h"
#include "DevCommons.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionListenerInterface.h"
#include "Perception/AIPerceptionSystem.h"
#include "AISenseEvent_AADamage.h"


bool FAISenseAADamageEvent::IsValid() const
{
	bool bIsValid = true;

	bIsValid |= (DamageEvent.Target != nullptr);
	bIsValid |= (DamageEvent.HitLocation != FVector::ZeroVector);

	return bIsValid;
}

float FAISenseAADamageEvent::GetEventStrengthForStimulus() const
{
	// TODO: Determine if this is necessary, or if we should keep all events identical in strength.
	return 1.0f;
}

IAIPerceptionListenerInterface* FAISenseAADamageEvent::GetTargetActorAsPerceptionListener() const
{
	IAIPerceptionListenerInterface* Listener = nullptr;
	AActor* Target = DamageEvent.Target;

	if (Target)
	{
		Listener = Cast<IAIPerceptionListenerInterface>(Target);
		if (Listener == nullptr)
		{
			APawn* ListenerAsPawn = Cast<APawn>(Target);
			if (ListenerAsPawn)
			{
				Listener = Cast<IAIPerceptionListenerInterface>(ListenerAsPawn->GetController());
			}
		}
	}

	return Listener;
}


void UAISense_AADamage::RegisterEvent(const FAISenseAADamageEvent& Event)
{
	if (Event.IsValid())
	{
		RegisteredEvents.Add(Event);

		RequestImmediateUpdate();
	}
	else
	{
		UE_LOG(VSLog, Warning, TEXT("UAISense_AADamage::RegisterEvent called with invalid event"));
	}

}

void UAISense_AADamage::RegisterWrappedEvent(UAISenseEvent& PerceptionEvent)
{
	UAISenseEvent_AADamage* DamageEvent = Cast<UAISenseEvent_AADamage>(&PerceptionEvent);
	ensure(DamageEvent);
	if (DamageEvent)
	{
		RegisterEvent(DamageEvent->Event);
	}
}

void UAISense_AADamage::ReportDamageEvent(UObject* WorldContextObject, FAAResolvedDamageEvent Event)
{
	UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(WorldContextObject);
	if (PerceptionSystem)
	{
		FAISenseAADamageEvent DamageEvent(Event);
		PerceptionSystem->OnEvent<FAISenseAADamageEvent, FAISenseAADamageEvent::FSenseClass>(Event);
	}
}

float UAISense_AADamage::Update()
{
	AIPerception::FListenerMap& ListenersMap = *GetListeners();

	for (const FAISenseAADamageEvent& Event : RegisteredEvents)
	{
		IAIPerceptionListenerInterface* PerceptionListener = Event.GetTargetActorAsPerceptionListener();
		if (!PerceptionListener)
		{
			continue;
		}

		UAIPerceptionComponent* PerceptionComponent = PerceptionListener->GetPerceptionComponent();
		if (PerceptionComponent != nullptr && PerceptionComponent->GetListenerId().IsValid())
		{
			// We assert if it fails
			FPerceptionListener& Listener = ListenersMap[PerceptionComponent->GetListenerId()];

			if (Listener.HasSense(GetSenseID()))
			{
				const FAAResolvedDamageEvent& EventData = Event.DamageEvent;

				const FAIStimulus Stimulus(*this, Event.GetEventStrengthForStimulus(), EventData.HitLocation, EventData.Target->GetActorLocation(), FAIStimulus::SensingSucceeded, NAME_None);

				Listener.RegisterStimulus(EventData.Source, Stimulus);
			}
		}
	}

	RegisteredEvents.Reset();

	// We only need updates when requested through RequestImmediateUpdate
	return SuspendNextUpdate;
}
