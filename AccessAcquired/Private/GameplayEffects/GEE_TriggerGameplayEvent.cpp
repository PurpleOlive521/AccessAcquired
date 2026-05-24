// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayEffects/GEE_TriggerGameplayEvent.h"
#include "GameplayEventSubsystem.h"
#include "GameplayEvent.h"
#include "GameplaySystemComponent.h"

void UGEE_TriggerGameplayEvent::PostApply(const FGameplayEffectExecutorParams& Params, const FGameplaySystemSnapshot& Snapshot) const
{
	Super::PostApply(Params, Snapshot);

	EXPAND_EXECUTOR_PARAMS(Params, GameplaySystem, ActiveEffect, GameplayEffect);

	if (AActor* Owner = GameplaySystem->GetOwner())
	{
		if (UGameplayEventSubsystem* EventSubsystem = UGameplayEventSubsystem::Get(GameplaySystem))
		{
			for (auto EventClass : OnApplyEvents)
			{
				FGameplayEventHandle Handle =EventSubsystem->TriggerEvent(EventClass, Owner);

				if (Handle.IsValid())
				{
					ActiveEffect->ActivatedEvents.Add(Handle);
				}
			}
		}
	}
}

void UGEE_TriggerGameplayEvent::PostRemove(const FGameplayEffectExecutorParams& Params, const FGameplaySystemSnapshot& Snapshot) const
{
	EXPAND_EXECUTOR_PARAMS(Params, GameplaySystem, ActiveEffect, GameplayEffect);

	if (AActor* Owner = GameplaySystem->GetOwner())
	{
		if (UGameplayEventSubsystem* EventSubsystem = UGameplayEventSubsystem::Get(GameplaySystem))
		{
			if (bAbortApplyEventsOnRemove)
			{
				EventSubsystem->EndEventsByHandles(ActiveEffect->ActivatedEvents);
			}

			for (auto EventClass : OnRemoveEvents)
			{
				EventSubsystem->TriggerEvent(EventClass, Owner);
			}
		}
	}

	Super::PostRemove(Params, Snapshot);
}
