// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "GA_ModeSwitch.h"

#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "PlayerCharacter.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "GameplaySystemBlueprintLibrary.h"
#include "DevCommons.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


UGA_ModeSwitch::UGA_ModeSwitch()
{
	ActivationBlockedTags.AddTag(GAMEPLAYTAG_Status_PerformingAction);
	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_PrimaryAction);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_Dash);
}

bool UGA_ModeSwitch::CheckAbilityRequirements_Implementation() const
{
	const bool ParentSucceded = Super::CheckAbilityRequirements_Implementation();

	if (!ParentSucceded)
	{
		return false;
	}

	check(GetOwningComponent());

	const FGameplaySystemActorInfo* ActorInfo = GetOwningComponent()->GetActorInfo();

	// Check if we are midair
	const UCharacterMovementComponent* MovementComp = ActorInfo->MovementComponent.Get();
	if (!MovementComp || !MovementComp->IsWalking())
	{
		return false;
	}

	// Check that we have the mode available
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningActor()))
	{
		if (!PlayerCharacter->HasMode(TargetMode))
		{
			return false;
		}
	}

	return true; 
}

bool UGA_ModeSwitch::ApplyAbilityRequirements_Implementation()
{
	const bool bSuperPassed = Super::ApplyAbilityRequirements_Implementation();

	if (!bSuperPassed)
	{
		return false;
	}

	return true;
}

void UGA_ModeSwitch::ActivateAbility_Implementation(FActiveGameplayAbility& OutActiveGameplayAbility)
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_CanPerfectDodge);

	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningActor()))
	{
		PlayerCharacter->SetModeInternal(TargetMode);
	}

	// Apply GameplayEffects if we have more than 75% Energy
	const float Energy = UGameplaySystemBlueprintLibrary::GetEnergyAsPercentage(GameplaySystem);
	if (HighEnergyActivationEffect && Energy >= 0.75f)
	{
		const FGameplayTag ModeAsTag = UGameplayUtilityBlueprintLibrary::GetModeAsGameplayTag(TargetMode);

		// Make sure we don't end up stacking activation-effects for the same mode.
		GameplaySystem->RemoveAllGameplayEffectsWithTag(ModeAsTag);

		FGameplayEffectHandle Handle;

		GameplaySystem->AddGameplayEffectFromType(HighEnergyActivationEffect, Handle);
	}

	// --- Play AnimMontage

	const FGameplaySystemActorInfo* ActorInfo = GameplaySystem->GetActorInfo();

	if (UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance())
	{
		const float AnimDuration = PlayMontage(AnimationMontage, 1.0f);

		OutActiveGameplayAbility.Duration = AnimDuration;

		OnModeSwitchStarted();
	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("No AnimInstance available for GameplayAbility."));
	}
}

void UGA_ModeSwitch::ApplyAbilityEndedModifiers_Implementation()
{
	Super::ApplyAbilityEndedModifiers_Implementation();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_CanPerfectDodge);
}

void UGA_ModeSwitch::RemoveAbilityEndedModifiers_Implementation()
{
	Super::RemoveAbilityEndedModifiers_Implementation();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_CanPerfectDodge);
}

void UGA_ModeSwitch::OnMontageBlendingOut(UAnimMontage* Montage, bool bWasInterrupted)
{
	TryApplyAbilityEndedModifiers();

	OnModeSwitchEnded(bWasInterrupted);
}