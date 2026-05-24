// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "DestructibleBase.h"
#include "GameplayMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTagMessageDefines.h"

ADestructibleBase::ADestructibleBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADestructibleBase::OnDestructed(AActor* InInstigator)
{
	// Disable all collisions for the actor
	SetActorEnableCollision(false);

	// Make Actor invisible
	GetRootComponent()->SetVisibility(false, true);

	FAARewardMessage RewardMsg;
	RewardMsg.GainedExperience = GainedExperience;
	RewardMsg.GainedIntel = GainedIntel;
	RewardMsg.Target = this;
	RewardMsg.Instigator = InInstigator;

	UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(this);
	MessageSystem->BroadcastMessage(GAMEPLAYTAG_Channels_Combat_Rewards, RewardMsg);

	K2_OnDestructed();

	Destroy();
}

FAAResolvedDamageEvent ADestructibleBase::ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent)
{
	FAAResolvedDamageEvent ResolvedDamageEvent(DamageEvent, this);
	ResolvedDamageEvent.EventStates.bWasEventIgnored = false;

	Health -= DamageEvent.StaggerDamage;
	ResolvedDamageEvent.DamageTaken = DamageEvent.StaggerDamage;

	if (Health <= 0)
	{
		OnDestructed(DamageEvent.DamageSource);
	}
	else
	{
		K2_OnDamageTaken();
	}

	return ResolvedDamageEvent;
}

bool ADestructibleBase::IsDead_Implementation()
{
	return bIsDestroyed;
}

ETeam ADestructibleBase::GetTeam_Implementation()
{
	return Team;
}

