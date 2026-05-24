// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayAbilities/GA_AIMeleeAttack.h"
#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "GameplayEffect.h"
#include "DevCommons.h"
#include "DevelopmentTypes.h"
#include "MeshAccessorInterface.h"
#include "WeaponComponent.h"
#include "ModeComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


UGA_AIMeleeAttack::UGA_AIMeleeAttack()
{
	ActivationBlockedTags.AddTag(GAMEPLAYTAG_Status_PerformingAction);
	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	InstancingPolicy = EInstancingPolicy::EIP_InstancedPerActor;
}

bool UGA_AIMeleeAttack::CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const
{
	FAIL_ON_FAILED_SUPER(CheckAbilityRequirements(ActivationData));

	// Not allowed to be performed mid-air
	const FGameplaySystemActorInfo* ActorInfo = GetOwningComponent()->GetActorInfo();
	const UCharacterMovementComponent* MovementComp = ActorInfo->MovementComponent.Get();
	if (!MovementComp)
	{
		return false;
	}

	if (!MovementComp->IsWalking())
	{
		return false;
	}

	return true;
}

bool UGA_AIMeleeAttack::ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData)
{
	FAIL_ON_FAILED_SUPER(ApplyAbilityRequirements(ActivationData));

	return true;
}

void UGA_AIMeleeAttack::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility(ActivationData, OutActiveGameplayAbility); // Assigns properties from GA_AIActionBase

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);

	// Animation should be handled in Blueprint with GameplayTasks
}

void UGA_AIMeleeAttack::CancelAbility()
{
	Super::CancelAbility();
}

void UGA_AIMeleeAttack::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);

}

void UGA_AIMeleeAttack::RemoveAbilityEndedModifiers()
{
	Super::RemoveAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
}

FPlayMontageAndWaitParams UGA_AIMeleeAttack::GetPlayMontageParams() const
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent();

	FPlayMontageAndWaitParams PlayParams;
	PlayParams.PlayRate = GameplaySystem->GetAttributeValue(EAttributeType::EAT_AttackSpeed, EAttributeValue::EAV_CurrentValue);

	return PlayParams;
}
