// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayAbilities/GA_ChargedHeavyAttack.h"

#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "CharacterBase.h"
#include "DevCommons.h"
#include "DevelopmentTypes.h"
#include "MeshAccessorInterface.h"
#include "WeaponComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GAT_WaitDelay.h"


UGA_ChargedHeavyAttack::UGA_ChargedHeavyAttack()
{
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	ActivationBlockedTags.AddTag(GAMEPLAYTAG_Status_PerformingAction);

	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_Dash);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Status_ActionCancellable);

	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	InstancingPolicy = EInstancingPolicy::EIP_InstancedPerActor;

	bRemoveCooldownWhenCancelled = true;
}

bool UGA_ChargedHeavyAttack::CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const
{
	FAIL_ON_FAILED_SUPER(CheckAbilityRequirements(ActivationData));

	// Not allowed to be performed mid-air
	const FGameplaySystemActorInfo* ActorInfo = GetOwningComponent()->GetActorInfo();
	if (const UCharacterMovementComponent* MovementComp = ActorInfo->MovementComponent.Get())
	{
		if (!MovementComp->IsWalking())
		{
			return false;
		}
	}

	return true;
}

bool UGA_ChargedHeavyAttack::ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData)
{
	FAIL_ON_FAILED_SUPER(ApplyAbilityRequirements(ActivationData));

	return true;
}

void UGA_ChargedHeavyAttack::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility(ActivationData, OutActiveGameplayAbility);

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);

	FGameplayEffectHandle Handle;
	for (const TSubclassOf<UGameplayEffect> GameplayEffect : ChargeEffects)
	{
		GameplaySystem->AddGameplayEffectFromType(GameplayEffect, Handle, GetOwningActor());
		ChargeEffectHandles.Add(Handle);
	}

	// Reset parameters
	CurrentBreakpoint = 0;

	// Animation should be handled in Blueprint with GameplayTasks
}

void UGA_ChargedHeavyAttack::CancelAbility()
{
	RemoveModifiers();

	Super::CancelAbility();
}

void UGA_ChargedHeavyAttack::EndAbility()
{
	RemoveModifiers();

	Super::EndAbility();
}

void UGA_ChargedHeavyAttack::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UGA_ChargedHeavyAttack::RemoveAbilityEndedModifiers()
{
	Super::RemoveAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
}

FPlayMontageAndWaitParams UGA_ChargedHeavyAttack::GetPlayMontageParams() const
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent();

	FPlayMontageAndWaitParams PlayParams;
	PlayParams.PlayRate = 1.0f;

	return PlayParams;
}

void UGA_ChargedHeavyAttack::OnChargingFinished(float ChargingProgress)
{
	const float FinalProgress = FMath::Clamp(ChargingProgress / MaxRewardedChargingTime, 0.0f, 1.0f);
	const float FinalMultiplier = MaxMultiplier * FinalProgress;

	if(DamageModifier)
	{
		if (UHitboxSourceComponent* HitboxSource = GetOwningActor()->GetComponentByClass<UHitboxSourceComponent>())
		{
			UDamageEventModifier* AppliedModifier = HitboxSource->AddDamageEventModifier(DamageModifier);

			AppliedModifier->DamageScalar = FinalMultiplier;
			AppliedModifier->ChargeScalar = FinalMultiplier;
			AppliedModifier->KnockbackScalar = FinalProgress;
		}
	}
}

void UGA_ChargedHeavyAttack::OnChargingStarted()
{
	WaitForNextBreakpoint();
	return;
}

void UGA_ChargedHeavyAttack::OnChargingBreakpointReached()
{
	CurrentBreakpoint++;
	K2_OnChargingBreakpointReached(CurrentBreakpoint);

	WaitForNextBreakpoint();
}

void UGA_ChargedHeavyAttack::WaitForNextBreakpoint()
{
	if (CurrentBreakpoint < TrackedChargingBreakpoints.Num())
	{
		float PreviousBreakpoint = 0.0f;
		if (CurrentBreakpoint - 1 >= 0)
		{
			PreviousBreakpoint = TrackedChargingBreakpoints[CurrentBreakpoint - 1];
		}

		const float NextBreakpoint = TrackedChargingBreakpoints[CurrentBreakpoint];
		const float BreakpointDuration = FMath::Clamp(NextBreakpoint - PreviousBreakpoint, 0.0f, 1.0f) * MaxRewardedChargingTime;
		UGAT_WaitDelay* Task = UGAT_WaitDelay::WaitDelay(this, BreakpointDuration);
		Task->OnWaitDelayFinishDelegate.AddUniqueDynamic(this, &UGA_ChargedHeavyAttack::OnChargingBreakpointReached);

		// Native activation of GameplayTasks is explicit, and not handled automatically through the custom K2 node.
		UGameplaySystemComponent* Component = GetOwningComponent();
		if (Component)
		{
			FGameplayResourceSet Resources = FGameplayResourceSet::NoResources();
			UGameplayTasksComponent::RunGameplayTask(*Component, *Task, 0U, Resources, Resources);
		}
	}
}

void UGA_ChargedHeavyAttack::RemoveModifiers()
{
	if (UGameplaySystemComponent* Component = GetOwningComponent())
	{
		Component->RemoveGameplayEffectsByHandles(ChargeEffectHandles);
		ChargeEffectHandles.Empty();
	}

	if (UHitboxSourceComponent* HitboxSource = GetOwningActor()->GetComponentByClass<UHitboxSourceComponent>())
	{
		HitboxSource->RemoveDamageEventModifierByType(DamageModifier);
	}
}
