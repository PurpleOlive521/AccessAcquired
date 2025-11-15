// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "GA_DamageTaken.h"
#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "DevCommons.h"


UGA_DamageTaken::UGA_DamageTaken()
{
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_PrimaryAction);
	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_PrimaryAction);
}

bool UGA_DamageTaken::ApplyAbilityRequirements_Implementation()
{
	const bool ParentSucceeded = Super::ApplyAbilityRequirements_Implementation();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	return true;
}

void UGA_DamageTaken::ActivateAbility_Implementation(FActiveGameplayAbility& OutActiveGameplayAbility)
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);

	FGameplayEffectHandle Handle;
	for (const TSubclassOf<UGameplayEffect> GameplayEffect : OnActivationEffects)
	{
		GameplaySystem->AddGameplayEffectFromType(GameplayEffect, Handle);
	}

	// Play Hit Animation
	const FGameplaySystemActorInfo* ActorInfo = GameplaySystem->GetActorInfo();
	if (UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance())
	{
		const int RandomIndex = FMath::RandRange(0, HitAnimations.Num() - 1);

		const float AnimDuration = PlayMontage(HitAnimations[RandomIndex], 1.0f);

		OutActiveGameplayAbility.Duration = AnimDuration;

		K2_PlayActivationEffects();
	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("No AnimInstance available for GameplayAbility."));
	}
}

void UGA_DamageTaken::ApplyAbilityEndedModifiers_Implementation()
{
	Super::ApplyAbilityEndedModifiers_Implementation();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UGA_DamageTaken::RemoveAbilityEndedModifiers_Implementation()
{
	Super::RemoveAbilityEndedModifiers_Implementation();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UGA_DamageTaken::OnMontageBlendingOut(UAnimMontage* Montage, bool bWasInterrupted)
{
	TryApplyAbilityEndedModifiers();
}
