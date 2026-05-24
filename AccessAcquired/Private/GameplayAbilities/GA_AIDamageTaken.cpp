// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayAbilities/GA_AIDamageTaken.h"
#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "DevCommons.h"
#include "DevelopmentTypes.h"
#include "EnemyAIControllerBase.h"


UGA_AIDamageTaken::UGA_AIDamageTaken()
{
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);
	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);
}

bool UGA_AIDamageTaken::CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const
{
	FAIL_ON_FAILED_SUPER(CheckAbilityRequirements(ActivationData));

	return true;
}

bool UGA_AIDamageTaken::ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData)
{
	FAIL_ON_FAILED_SUPER(ApplyAbilityRequirements(ActivationData));

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	return true;
}

void UGA_AIDamageTaken::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility(ActivationData, OutActiveGameplayAbility);

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);

	// Animation should be handled in Blueprint with GameplayTasks
}

void UGA_AIDamageTaken::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UGA_AIDamageTaken::RemoveAbilityEndedModifiers()
{
	Super::RemoveAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
}

UAnimMontage* UGA_AIDamageTaken::GetRandomMontage() const
{
	if (HitAnimations.IsEmpty())
	{
		return nullptr;
	}

	const int RandomIndex = FMath::RandRange(0, HitAnimations.Num() - 1);

	return HitAnimations[RandomIndex];
}

void UGA_AIDamageTaken::OnAnimationFinished()
{
	TryApplyAbilityEndedModifiers();

	if (APawn* Pawn = Cast<APawn>(GetOwningActor_Checked()))
	{
		if (AEnemyAIControllerBase* AIController = Cast<AEnemyAIControllerBase>(Pawn->GetController()))
		{
			AIController->RemoveState(FrozenState);
		}
	}
}