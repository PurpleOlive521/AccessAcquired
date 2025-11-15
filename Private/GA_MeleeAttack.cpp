// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "GA_MeleeAttack.h"

#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "PlayerCharacter.h"
#include "TargetLockingComponent.h"
#include "GameplayEffect.h"
#include "DevCommons.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


UGA_MeleeAttack::UGA_MeleeAttack()
{
	ActivationBlockedTags.AddTag(GAMEPLAYTAG_Status_PerformingAction);
	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_PrimaryAction);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_Dash);
}

bool UGA_MeleeAttack::CheckAbilityRequirements_Implementation() const
{
	const bool bPassedBaseRequirements = Super::CheckAbilityRequirements_Implementation();

	if (!bPassedBaseRequirements)
	{
		return false;
	}

	UGameplaySystemComponent* Component = GetOwningComponent_Checked();

	const bool bHasEnergy = Component->GetAttributeValue(EAttributeType::EAT_Energy) >= EnergyCost;
	if (!bHasEnergy)
	{
		return false;
	}
	
	// Not allowed to be performed mid-air
	const FGameplaySystemActorInfo* ActorInfo = GetOwningComponent()->GetActorInfo();
	if (const UCharacterMovementComponent* MovementComp = ActorInfo->MovementComponent.Get(); MovementComp->IsWalking())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool UGA_MeleeAttack::ApplyAbilityRequirements_Implementation()
{
	const bool bSuperPassed = Super::ApplyAbilityRequirements_Implementation();

	if (!bSuperPassed)
	{
		return false;
	}

	AActor* Owner = GetOwningActor_Checked();

	APlayerCharacter* Player = CastChecked<APlayerCharacter>(Owner);
	const bool bHadEnergy = Player->RequestEnergyUse(EnergyCost);

	return bHadEnergy;
}

void UGA_MeleeAttack::ActivateAbility_Implementation(FActiveGameplayAbility& OutActiveGameplayAbility)
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);

	FGameplayEffectHandle Handle;
	for (const TSubclassOf<UGameplayEffect> GameplayEffect : AttackStartedEffects)
	{
		GameplaySystem->AddGameplayEffectFromType(GameplayEffect, Handle);
	}

	// Setup the MotionWarping points before starting the AnimMontage
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningActor()))
	{
		if (UTargetLockingComponent* TargetLockingComponent = PlayerCharacter->GetComponentByClass<UTargetLockingComponent>())
		{
			// We have a Actor to target
			if (AActor* TargetActor = TargetLockingComponent->GetTargetedActor())
			{
				TargetLockingComponent->WarpTowardsActor(TargetActor, bEnableDebugLogs);
			}
			else
			{
				TargetLockingComponent->WarpToClosestTarget(PlayerCharacter->GetCameraAdjustedMovementInput(), bEnableDebugLogs);
			}
		}	
		
		// Construct the necessary data to resolve any damage events this attack causes
		FAADamageEvent AttackDamageEvent;
		AttackDamageEvent.Damage =			GameplaySystem->GetAttributeValue(EAttributeType::EAT_Damage);
		AttackDamageEvent.AppliedCharge =	GameplaySystem->GetAttributeValue(EAttributeType::EAT_AppliedCharge);
		AttackDamageEvent.Level =			GameplaySystem->GetEntityLevel();
		AttackDamageEvent.DamageType =		PlayerCharacter->GetMode();
		AttackDamageEvent.DamageSource =	PlayerCharacter;
		AttackDamageEvent.DamageMultiplier =DamageMultiplier;
		AttackDamageEvent.StaggerDamage =	StaggerDamage;

		PlayerCharacter->AssignDamageEvent(AttackDamageEvent);

		// Informs the player to increment the counter, allowing the player to chain attacks together
		PlayerCharacter->IncrementAttackCounter();
	}

	const FGameplaySystemActorInfo* ActorInfo = GameplaySystem->GetActorInfo();

	if (UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance())
	{
		// If we are mid-action we want to start past the windup of the animation to make the animation feel more fluid.
		FName EndSection = FName();
		if (GameplaySystem->HasTag(GAMEPLAYTAG_Status_RecentlyPerformedAction))
		{
			EndSection = FName(TEXT("Windup"));
		}

		const float PlayRate = GameplaySystem->GetAttributeValue(EAttributeType::EAT_AttackSpeed);
		const float AnimDuration = PlayMontage(AnimationMontage, PlayRate, FName(), EndSection);

		OutActiveGameplayAbility.Duration = AnimDuration;

		// Blueprint hook
		OnAttackStarted();
	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("No AnimInstance available for GameplayAbility."));
	}
}

void UGA_MeleeAttack::CancelAbility_Implementation()
{
	Super::CancelAbility_Implementation();
}

void UGA_MeleeAttack::ApplyAbilityEndedModifiers_Implementation()
{
	Super::ApplyAbilityEndedModifiers_Implementation();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);

	FGameplayEffectHandle Handle;
	for (const TSubclassOf<UGameplayEffect> GameplayEffect : AttackEndedEffects)
	{
		GameplaySystem->AddGameplayEffectFromType(GameplayEffect, Handle);
		AttackEndedEffectHandles.Emplace(Handle);
	}
}

void UGA_MeleeAttack::RemoveAbilityEndedModifiers_Implementation()
{
	Super::RemoveAbilityEndedModifiers_Implementation();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);

	for (const FGameplayEffectHandle& Handle : AttackEndedEffectHandles)
	{
		GameplaySystem->RemoveGameplayEffectByHandle(Handle);
	}
}

FString UGA_MeleeAttack::ToString() const
{
	const FString ParentString = Super::ToString();

	FString DisplayInfo = ParentString + FString::Printf(TEXT("EnergyCost : %.2f"), EnergyCost);

	return DisplayInfo;
}

FString UGA_MeleeAttack::ToStringWithDebugTags() const
{
	const FString ParentString = Super::ToStringWithDebugTags();

	FString DisplayInfo = ParentString + FString::Printf(TEXT("EnergyCost : %.2f"), EnergyCost);

	return DisplayInfo;
}

void UGA_MeleeAttack::OnMontageBlendingOut(UAnimMontage* Montage, bool bWasInterrupted)
{
	TryApplyAbilityEndedModifiers();

	// Blueprint hook
	OnAttackEnded(bWasInterrupted);
}
