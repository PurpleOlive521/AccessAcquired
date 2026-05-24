// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GA_Dash.h"

#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "DevelopmentTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DevCommons.h"

UGA_Dash::UGA_Dash()
{
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_Dash);
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	ActivationBlockedTags.AddTag(GAMEPLAYTAG_Status_PerformingAction);

	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_Jump);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);
}

bool UGA_Dash::CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const
{
	FAIL_ON_FAILED_SUPER(CheckAbilityRequirements(ActivationData));

	const FGameplaySystemActorInfo* ActorInfo = GetOwningComponent()->GetActorInfo();
	if (const UCharacterMovementComponent* MovementComp = ActorInfo->MovementComponent.Get())
	{
		// In air
		if (!MovementComp->IsWalking())
		{
			if (GetOwningComponent()->HasTag(GAMEPLAYTAG_Player_AirDashed))
			{
				return false;
			}
		}
	}

	return true;
}

bool UGA_Dash::ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData)
{
	FAIL_ON_FAILED_SUPER(ApplyAbilityRequirements(ActivationData));

	return true;
}

void UGA_Dash::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility(ActivationData, OutActiveGameplayAbility);

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_Untargetable);

	const FGameplaySystemActorInfo* ActorInfo = GameplaySystem->GetActorInfo();
	if (UCharacterMovementComponent* MovementComp = ActorInfo->MovementComponent.Get())
	{
		// In air
		if (!MovementComp->IsWalking())
		{
			GameplaySystem->AddTag(GAMEPLAYTAG_Player_AirDashed);
		}

		// Disable gravity while dashing
		MovementComp->SetMovementMode(EMovementMode::MOVE_Flying);
	}

	// The dash is currently controlled by a blendspace for the actual animation, so we need to explicitly claim any incoming AnimNotifys as ours.
	GameplaySystem->GetAnimMontageInfo()->SetOverrideAbility(this);
}

void UGA_Dash::CancelAbility()
{
	EnableGravity();

	Super::CancelAbility();
}

void UGA_Dash::EndAbility()
{
	EnableGravity();

	Super::EndAbility();
}

void UGA_Dash::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_Untargetable);
}

void UGA_Dash::RemoveAbilityEndedModifiers()
{
	Super::RemoveAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_Untargetable);
}

void UGA_Dash::EnableGravity()
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	// Reset gravity
	const FGameplaySystemActorInfo* ActorInfo = GameplaySystem->GetActorInfo();
	if (UCharacterMovementComponent* MovementComp = ActorInfo->MovementComponent.Get())
	{
		if (MovementComp->MovementMode == EMovementMode::MOVE_Flying)
		{
			FFindFloorResult Floor;
			MovementComp->FindFloor(GetOwningActor()->GetActorLocation(), Floor, true /* bCanUseCachedLocation */);

			// We are close to or on the floor
			if (Floor.bWalkableFloor)
			{
				MovementComp->SetMovementMode(EMovementMode::MOVE_Walking);
			}
			else
			{
				MovementComp->SetMovementMode(EMovementMode::MOVE_Falling);
			}
		}
	}
}
