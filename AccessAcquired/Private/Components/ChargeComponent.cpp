// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "ChargeComponent.h"

#include "GameplaySystemComponent.h"
#include "GameplayTagSystem.h"
#include "DevCommons.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "ReceiveDamageEventInterface.h"
#include "GameplayTagDefines.h"

UChargeComponent::UChargeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

FAAResolvedDamageEvent UChargeComponent::ResolveDamageEvent(const FAADamageEvent& DamageEvent)
{
	// Setting the default, base behaviour
	FAAResolvedDamageEvent ResolvedDamageEvent(DamageEvent, GetOwner());
	ResolvedDamageEvent.EventStates.bWasEventIgnored = false;
	ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = true;
	ResolvedDamageEvent.AppliedKnockback = DamageEvent.Knockback;

	if(IReceiveDamageEventInterface::Execute_IsDead(GetOwner()) == true)
	{
		ResolvedDamageEvent.ResolveAsAlreadyDead();
		return ResolvedDamageEvent;
	}

	// --- Check against tags

	if(GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_KnockbackImmunity))
	{
		ResolvedDamageEvent.AppliedKnockback = 0.0f;
	}

	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_Untargetable))
	{
		ResolvedDamageEvent.ResolveAsUntargetable();
		return ResolvedDamageEvent;
	}

	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_Invincible))
	{
		ResolvedDamageEvent.ResolveAsInvincible(DamageEvent.EventFlags.bAllowHitAnim);
		return ResolvedDamageEvent;
	}

	if (!DamageEvent.EventFlags.bIgnoreIFrames)
	{
		if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_IFrames))
		{
			ResolvedDamageEvent.ResolveAsInvincible(DamageEvent.EventFlags.bAllowHitAnim);
			return ResolvedDamageEvent;
		}
	}

	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_HyperArmor))
	{
		ResolvedDamageEvent.ResolveAsUninterruptible();
	}
	else
	{
		// --- Resolve Stagger
		const float StaggerThreshold = GameplaySystem->GetAttributeValue(EAttributeType::EAT_StaggerThreshold, EAttributeValue::EAV_CurrentValue);

		// Significantly higher than self
		if (DamageEvent.Level > GameplaySystem->GetEntityLevel() + SIGNIFICANT_LEVEL_DIFFERENCE)
		{
			ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = true;
		}
		// Significantly less than self
		else if (DamageEvent.Level < GameplaySystem->GetEntityLevel() - SIGNIFICANT_LEVEL_DIFFERENCE)
		{
			ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = false;
		}
		// Not significant, use StaggerThreshold
		else if (DamageEvent.StaggerDamage >= StaggerThreshold)
		{
			ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = true;
		}

		if (DamageEvent.EventFlags.bForceHitAnim)
		{
			ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = true;
		}

		if (!DamageEvent.EventFlags.bAllowHitAnim)
		{
			ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = false;
		}
	}

	// --- Applying damage

	const float RawDamageReduction = GameplaySystem->GetAttributeValue(EAttributeType::EAT_DamageReduction, EAttributeValue::EAV_CurrentValue);
	const float FinalDamageReduction = FMath::Abs(1 - (RawDamageReduction / 100));
	const float WeaknessMultiplier = GameplaySystem->GetAttributeValue(UGameplayUtilityBlueprintLibrary::GetCorrespondingWeakness(DamageEvent.DamageType), EAttributeValue::EAV_CurrentValue);

	if (WeaknessMultiplier - 1 >= HIT_WEAKNESS_THRESHOLD)
	{
		ResolvedDamageEvent.EventStates.bHitWeakness = true;
	}

	float TotalDamage = DamageEvent.Damage * WeaknessMultiplier * FinalDamageReduction;
	float TotalCharge = DamageEvent.AppliedCharge * WeaknessMultiplier * FinalDamageReduction;

	// Overcharged state, damage is multiplied
	if (ChargeState != EChargeState::ECS_Normal)
	{
		const float OverchargedMultiplier = GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverchargedDamageMultiplier, EAttributeValue::EAV_CurrentValue);
		TotalDamage *= OverchargedMultiplier;
		GameplaySystem->ModifyAttributeValue(EAttributeType::EAT_Health, EAttributeValue::EAV_BaseValue, -TotalDamage);
		ResolvedDamageEvent.DamageTaken = TotalDamage;
	}
	// Normal state
	else
	{
		GameplaySystem->ModifyAttributeValue(EAttributeType::EAT_Health, EAttributeValue::EAV_BaseValue, -TotalDamage);
		ResolvedDamageEvent.DamageTaken = TotalDamage;

		GameplaySystem->ModifyAttributeValue(EAttributeType::EAT_Charge, EAttributeValue::EAV_BaseValue, TotalCharge);
		ResolvedDamageEvent.ChargeAdded = TotalCharge;
	}

	// --- Resolve Overcharge 
	
	// Did the damage received overcharge this?
	if (ChargeState == EChargeState::ECS_Normal)
	{
		const float MaxCharge = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge, EAttributeValue::EAV_CurrentValue);
		
		if (IsOvercharged())
		{
			GameplaySystem->SetAttributeValue(EAttributeType::EAT_Charge, EAttributeValue::EAV_BaseValue, MaxCharge);

			// Set directly to ensure it's not delayed until the next Tick
			SetChargeState(EChargeState::ECS_Overcharged);

			ResolvedDamageEvent.EventStates.bWasOvercharged = true;
		}
	}

	// Apply GameplayEffects
	FGameplayEffectHandle Handle;
	for (TSubclassOf<UGameplayEffect> Effect : DamageEvent.EffectsOnTarget)
	{
		GameplaySystem->AddGameplayEffectFromType(Effect, Handle, DamageEvent.DamageSource);
	}

	// Apply GameplayEffectStackModifiers
	for (const auto& GameplayEffectModifier : DamageEvent.StackModifiersOnTarget)
	{
		GameplaySystem->ApplyGameplayEffectStackModifier(GameplayEffectModifier.GameplayEffect, GameplayEffectModifier.Modifier, DamageEvent.DamageSource);
	}

	return ResolvedDamageEvent;
}

void UChargeComponent::BeginPlay()
{
	Super::BeginPlay();

	GameplaySystem = GetOwner()->FindComponentByClass<UGameplaySystemComponent>();

	if (!GameplaySystem)
	{
		UE_LOG(VSLog, Error, TEXT("ChargeComponent: Error - Could not find GameplaySystemComponent"));
		return;
	}
}

void UChargeComponent::NormalState(float DeltaTime)
{
	if (IsOvercharged())
	{
		SetChargeState(EChargeState::ECS_Overcharged);
	}
}

void UChargeComponent::OnEnterNormalState()
{
	GameplaySystem->SetAttributeValue(EAttributeType::EAT_Charge, EAttributeValue::EAV_BaseValue, 0.0f);
	OnRecoveredDelegate.Broadcast();
}

void UChargeComponent::RecoveringState(float DeltaTime)
{
	RecoveryTimeCounter += DeltaTime;

	// Slowly remove charge
	const float RecoveryTime = GameplaySystem->GetAttributeValue(EAttributeType::EAT_RecoveryTime, EAttributeValue::EAV_CurrentValue);
	const float MaxCharge = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge, EAttributeValue::EAV_CurrentValue);

	const float Alpha = RecoveryTimeCounter / RecoveryTime;
	float Charge = FMath::Lerp(MaxCharge, 0.0f, Alpha);
	GameplaySystem->SetAttributeValue(EAttributeType::EAT_Charge, EAttributeValue::EAV_BaseValue, Charge);

	const bool bIsRecovered = RecoveryTimeCounter >= RecoveryTime;
	if (bIsRecovered)
	{
		SetChargeState(EChargeState::ECS_Normal);
	}
}

void UChargeComponent::OnEnterRecoveringState()
{
	RecoveryTimeCounter = 0.0f;
}

// Increment time counter and check if it is time to recover
void UChargeComponent::OverchargedState(float DeltaTime)
{
	BrokenTimeCounter += DeltaTime;

	const float RecoveryDelay = GameplaySystem->GetAttributeValue(EAttributeType::EAT_RecoveryDelay, EAttributeValue::EAV_CurrentValue);
	const bool bDelayIsOver = BrokenTimeCounter > RecoveryDelay;
	if (bDelayIsOver)
	{
		SetChargeState(EChargeState::ECS_Recovering);
	}
}

void UChargeComponent::OnEnterOverchargedState()
{
	BrokenTimeCounter = 0.0f;

	const float MaxCharge = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge, EAttributeValue::EAV_CurrentValue);
	GameplaySystem->SetAttributeValue(EAttributeType::EAT_Charge, EAttributeValue::EAV_BaseValue, MaxCharge);

	OnOverchargedDelegate.Broadcast();
}

bool UChargeComponent::IsOvercharged() const
{
	const float Charge = GameplaySystem->GetAttributeValue(EAttributeType::EAT_Charge, EAttributeValue::EAV_CurrentValue);
	const float MaxCharge = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge, EAttributeValue::EAV_CurrentValue);

	return Charge >= MaxCharge;
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

