// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GA_MeleeAttack.h"

#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "PlayerCharacter.h"
#include "TargetLockingComponent.h"
#include "DevCommons.h"
#include "DevelopmentTypes.h"
#include "MeshAccessorInterface.h"
#include "WeaponComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayUtilityBlueprintLibrary.h"


UGA_MeleeAttack::UGA_MeleeAttack()
{
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	ActivationBlockedTags.AddTag(GAMEPLAYTAG_Status_PerformingAction);

	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Status_ActionCancellable);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_Dash);

	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	InstancingPolicy = EInstancingPolicy::EIP_InstancedPerActor;

	bRemoveCooldownWhenCancelled = true;
}

bool UGA_MeleeAttack::CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const
{
	FAIL_ON_FAILED_SUPER(CheckAbilityRequirements(ActivationData));

	// Not allowed to be performed mid-air
	const FGameplaySystemActorInfo* ActorInfo = GetOwningComponent()->GetActorInfo();

	if (not UGameplayUtilityBlueprintLibrary::IsVeryCloseToFloor(ActorInfo->GetCharacterMovement()))
	{
		return false;
	}

	return true;
}

bool UGA_MeleeAttack::ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData)
{
	FAIL_ON_FAILED_SUPER(ApplyAbilityRequirements(ActivationData));

	return true;
}

void UGA_MeleeAttack::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility(ActivationData, OutActiveGameplayAbility);

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);

	// Animation should be handled in Blueprint with GameplayTasks
}

void UGA_MeleeAttack::CancelAbility()
{
	Super::CancelAbility();
}

void UGA_MeleeAttack::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UGA_MeleeAttack::RemoveAbilityEndedModifiers()
{
	Super::RemoveAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
}

FPlayMontageAndWaitParams UGA_MeleeAttack::GetPlayMontageParams() const
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent();

	FPlayMontageAndWaitParams PlayParams;
	PlayParams.PlayRate = GameplaySystem->GetAttributeValue(EAttributeType::EAT_AttackSpeed, EAttributeValue::EAV_CurrentValue);

	return PlayParams;
}
