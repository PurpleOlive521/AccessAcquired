// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "ChargeComponent.h"

#include "GameplaySystemComponent.h"
#include "GameplayTagSystem.h"
#include "DevCommons.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "ReceiveDamageEventInterface.h"
#include "GameplayTagDefines.h"

// Sets default values for this component's properties
UChargeComponent::UChargeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

FAAResolvedDamageEvent UChargeComponent::ResolveDamageEvent(const FAADamageEvent& DamageEvent)
{
	// Setting the default, base behaviour
	FAAResolvedDamageEvent ResolvedDamageEvent(DamageEvent);
	ResolvedDamageEvent.EventStates.bWasEventIgnored = false;
	ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = true;

	if(IReceiveDamageEventInterface::Execute_IsDead(GetOwner()) == true)
	{
		ResolvedDamageEvent.ResolveAsAlreadyDead();
		return ResolvedDamageEvent;
	}

	// --- Check against tags

	// Take no damage, dont get interrupted
	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_Untargetable))
	{
		ResolvedDamageEvent.ResolveAsUntargetable();
		return ResolvedDamageEvent;
	}

	// Take no damage, still get interrupted
	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_Invincible))
	{
		ResolvedDamageEvent.ResolveAsInvincible();
		return ResolvedDamageEvent;
	}

	// Only check if we wont ignore it regardless of if IFrames are present
	if (!DamageEvent.bIgnoreIFrames)
	{
		if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_IFrames))
		{
			ResolvedDamageEvent.ResolveAsInvincible();
			return ResolvedDamageEvent;
		}
	}

	// Take damage, dont get interrupted
	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_HyperArmor))
	{
		ResolvedDamageEvent.ResolveAsUninterruptible();
	}
	else
	{
		// --- Resolve Stagger
		// When Player lvl > enemy lvl, it should always stagger self, while when player lvl < enemy lvl it should never stagger self
		const int LevelBuffer = 2;

		// Damage source level is significantly higher than self
		if (DamageEvent.Level > GameplaySystem->GetEntityLevel() + LevelBuffer)
		{
			ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = true;
		}
		// Damage source level is significantly less than self
		else if (DamageEvent.Level < GameplaySystem->GetEntityLevel() - LevelBuffer)
		{
			ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = false;
		}
		// Level difference is not signifiant, use StaggerThreshold
		else if (DamageEvent.StaggerDamage >= GameplaySystem->GetAttributeValue(EAttributeType::EAT_StaggerThreshold))
		{
			ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = true;
		}

	}

	// --- Applying damage

	const float DamageReduction = FMath::Abs(1 - (GameplaySystem->GetAttributeValue(EAttributeType::EAT_DamageReduction) / 100));
	const float WeaknessMultiplier = GameplaySystem->GetAttributeValue(UGameplayUtilityBlueprintLibrary::GetCorrespondingWeakness(DamageEvent.DamageType));

	if (WeaknessMultiplier - 1 >= 0.25f)
	{
		ResolvedDamageEvent.EventStates.bHitWeakness = true;
	}

	float TotalDamage = DamageEvent.Damage * DamageEvent.DamageMultiplier * WeaknessMultiplier * DamageReduction;
	float TotalCharge = DamageEvent.AppliedCharge * DamageEvent.DamageMultiplier * WeaknessMultiplier * DamageReduction;

	// Overcharged state, damage is multiplied
	if (ChargeState != EChargeState::ECS_Normal)
	{
		float OverchargedMultiplier = GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverchargedDamageMultiplier);
		TotalDamage *= OverchargedMultiplier;
		GameplaySystem->ModifyAttributeBaseValue(EAttributeType::EAT_Health, -TotalDamage);
		ResolvedDamageEvent.DamageTaken = TotalDamage;
	}
	// Normal state
	else
	{
		GameplaySystem->ModifyAttributeBaseValue(EAttributeType::EAT_Health, -TotalDamage);
		ResolvedDamageEvent.DamageTaken = TotalDamage;

		const bool bIsNotBroken = GameplaySystem->GetAttributeValue(EAttributeType::EAT_Charge) < GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge);
		if (bIsNotBroken)
		{
			GameplaySystem->ModifyAttributeBaseValue(EAttributeType::EAT_Charge, +TotalCharge);
			ResolvedDamageEvent.ChargeAdded = TotalCharge;
		}
	}

	// --- Resolve Overcharge 
	
	// Did the damage received overcharge this?
	if (ChargeState == EChargeState::ECS_Normal)
	{
		const bool bIsOvercharged = GameplaySystem->GetAttributeValue(EAttributeType::EAT_Charge) >= GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge);
		if (bIsOvercharged)
		{
			const float MaxCharge = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge);
			GameplaySystem->SetAttributeBaseValue(EAttributeType::EAT_Charge, MaxCharge);

			// Set directly to ensure it's not delayed until the next Tick
			SetChargeState(EChargeState::ECS_Overcharged);

			ResolvedDamageEvent.EventStates.bWasOvercharged = true;
		}
	}

	// Apply GameplayEffects
	FGameplayEffectHandle Handle;
	for (TSubclassOf<UGameplayEffect> Effect : DamageEvent.EffectsOnTarget)
	{
		GameplaySystem->AddGameplayEffectFromType(Effect, Handle);
	}

	// --- Clamping values
	GameplaySystem->ClampAttributeBaseValue(EAttributeType::EAT_Health, 0.0f, GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxHealth));
	GameplaySystem->ClampAttributeBaseValue(EAttributeType::EAT_Charge, 0.0f, GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge));

	// Ensure that we don't play a hit animation if we are overcharged, since the state has it's own animation
	if (ChargeState != EChargeState::ECS_Normal)
	{
		ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = false;
	}

	return ResolvedDamageEvent;
}

void UChargeComponent::BeginPlay()
{
	Super::BeginPlay();

	GameplaySystem = GetOwner()->FindComponentByClass<UGameplaySystemComponent>();

	if (!GameplaySystem)
	{
		UE_LOG(VSLog, Error, TEXT("PressureComponent: Error - Could not find GameplaySystemComponent"));
		return;
	}
}

void UChargeComponent::NormalState(float DeltaTime)
{
	const bool bIsBroken = GameplaySystem->GetAttributeValue(EAttributeType::EAT_Charge) >= GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge);
	if (bIsBroken)
	{
		SetChargeState(EChargeState::ECS_Overcharged);
	}
}

void UChargeComponent::OnEnterNormalState()
{
	GameplaySystem->SetAttributeBaseValue(EAttributeType::EAT_Charge, 0.0f);
	OnRecoveredDelegate.Broadcast();

	UE_LOG(VSLog, Log, TEXT("Enter normal"));

}

void UChargeComponent::RecoveringState(float DeltaTime)
{
	RecoveryTimeCounter += DeltaTime;

	// Slowly remove charge

	const float Alpha = RecoveryTimeCounter / GameplaySystem->GetAttributeValue(EAttributeType::EAT_RecoveryTime);
	float Charge = FMath::Lerp(GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge), 0.f, Alpha);
	GameplaySystem->SetAttributeBaseValue(EAttributeType::EAT_Charge, Charge);

	const bool bIsRecovered = RecoveryTimeCounter >= GameplaySystem->GetAttributeValue(EAttributeType::EAT_RecoveryTime);
	if (bIsRecovered)
	{
		SetChargeState(EChargeState::ECS_Normal);
	}
}

void UChargeComponent::OnEnterRecoveringState()
{
	RecoveryTimeCounter = 0.0f;
	UE_LOG(VSLog, Log, TEXT("Enter recovering"));

}

// Increment time counter and check if it is time to recover
void UChargeComponent::OverchargedState(float DeltaTime)
{
	BrokenTimeCounter += DeltaTime;

	const bool bDelayIsOver = BrokenTimeCounter > GameplaySystem->GetAttributeValue(EAttributeType::EAT_RecoveryDelay);
	if (bDelayIsOver)
	{
		SetChargeState(EChargeState::ECS_Recovering);
	}
}

void UChargeComponent::OnEnterOverchargedState()
{
	BrokenTimeCounter = 0.0f;

	const float MaxCharge = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge);
	GameplaySystem->SetAttributeBaseValue(EAttributeType::EAT_Charge, MaxCharge);

	OnOverchargedDelegate.Broadcast();
}

void UChargeComponent::SetChargeState(EChargeState InState)
{
	if (ChargeState == InState)
	{
		return;
	}

	ChargeState = InState;

	switch (ChargeState)
	{
	case EChargeState::ECS_Normal:
		OnEnterNormalState();
		break;
	case EChargeState::ECS_Recovering:
		OnEnterRecoveringState();
		break;
	case EChargeState::ECS_Overcharged:
		OnEnterOverchargedState();
		break;
	}
}

void UChargeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IReceiveDamageEventInterface::Execute_IsDead(GetOwner()) == true)
	{
		return;
	}

	switch (ChargeState)
	{
	case EChargeState::ECS_Overcharged:
		OverchargedState(DeltaTime);
		break;
	case EChargeState::ECS_Recovering:
		RecoveringState(DeltaTime);
		break;
	case EChargeState::ECS_Normal:
		NormalState(DeltaTime);
		break;
	}
}

