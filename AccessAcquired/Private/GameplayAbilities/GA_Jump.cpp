// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayAbilities/GA_Jump.h"
#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "DevelopmentTypes.h"
#include "GameFramework/Character.h"

UGA_Jump::UGA_Jump()
{
	InstancingPolicy = EInstancingPolicy::EIP_InstancedPerActor;

	ActivationBlockedTags.AddTag(GAMEPLAYTAG_Status_PerformingAction);
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_Jump);

	bIsCancellable = false;
}

bool UGA_Jump::CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const
{
	FAIL_ON_FAILED_SUPER(CheckAbilityRequirements(ActivationData));

	ACharacter* ControlledCharacter = Cast<ACharacter>(GetOwningActor());
	if (!ControlledCharacter || !ControlledCharacter->CanJump())
	{
		return false;
	}

	return true;
}

void UGA_Jump::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UGA_Jump::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UGA_Jump::RemoveAbilityEndedModifiers()
{
	Super::RemoveAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UGA_Jump::StartJump()
{
	ACharacter* ControlledCharacter = Cast<ACharacter>(GetOwningActor());
	if (ControlledCharacter && !ControlledCharacter->bPressedJump)
	{
		ControlledCharacter->Jump();
	}
}

void UGA_Jump::StopJump()
{
	ACharacter* ControlledCharacter = Cast<ACharacter>(GetOwningActor());
	if (ControlledCharacter && ControlledCharacter->bPressedJump)
	{
		ControlledCharacter->StopJumping();
	}
}
