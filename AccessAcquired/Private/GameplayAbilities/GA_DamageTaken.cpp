// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GA_DamageTaken.h"
#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "DevCommons.h"
#include "DevelopmentTypes.h"


UGA_DamageTaken::UGA_DamageTaken()
{
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	bIsAuthoritativeCancel = true;
}

bool UGA_DamageTaken::ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData)
{
	FAIL_ON_FAILED_SUPER(ApplyAbilityRequirements(ActivationData));

	return true;
}

void UGA_DamageTaken::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility(ActivationData, OutActiveGameplayAbility);

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);

	// Animation should be handled in Blueprint with GameplayTasks
}

void UGA_DamageTaken::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UGA_DamageTaken::RemoveAbilityEndedModifiers()
{
	Super::RemoveAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
}

UAnimMontage* UGA_DamageTaken::GetRandomMontage(const FRotator& Direction) const
{
	const TArray<UAnimMontage*>& AnimationBucket = GetAnimationBucket(Direction);

	if (AnimationBucket.IsEmpty())
	{
		return nullptr;
	}

	const int RandomIndex = FMath::RandRange(0, AnimationBucket.Num() - 1);

	return AnimationBucket[RandomIndex];
}

const TArray<UAnimMontage*>& UGA_DamageTaken::GetAnimationBucket(const FRotator& Direction) const
{
	const int32 Bucket = FMath::RoundToInt32(Direction.Yaw / 90.0f);

	switch (Bucket)
	{
		case(-2):
		case(2):
		{
			return BackwardsAnimations;
			break;
		}

		case(-1):
		{
			return LeftAnimations;
			break;
		}

		case(0):
		{
			return ForwardAnimations;
			break;
		}

		case(1):
		{
			return RightAnimations;
		}

		default:
		{
			checkNoEntry();
			return ForwardAnimations;
		}
	}
}
