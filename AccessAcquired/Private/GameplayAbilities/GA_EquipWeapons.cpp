// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayAbilities/GA_EquipWeapons.h"
#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "GameplayEffect.h"
#include "DevCommons.h"
#include "DevelopmentTypes.h"
#include "MeshAccessorInterface.h"
#include "WeaponComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UGA_EquipWeapons::UGA_EquipWeapons()
{
	ActivationBlockedTags.AddTag(GAMEPLAYTAG_Status_PerformingAction);
	ActivationBlockedTags.AddTag(GAMEPLAYTAG_Enemy_Status_EquippedWeapon);

	InstancingPolicy = EInstancingPolicy::EIP_InstancedPerActor;
}

bool UGA_EquipWeapons::CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const
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

void UGA_EquipWeapons::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility(ActivationData, OutActiveGameplayAbility);

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);

	WeaponComp = IMeshAccessorInterface::Execute_GetWeapon(GetOwningActor_Checked(), 0);
	if (!WeaponComp)
	{
		UE_LOG(VSLog, Error, TEXT("Activating Actor has no accessible WeaponComponent!"));
		return;
	}

	// Animation should be handled in Blueprint with GameplayTasks
}

void UGA_EquipWeapons::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);

	GameplaySystem->AddTag(GAMEPLAYTAG_Enemy_Status_EquippedWeapon);
}

void UGA_EquipWeapons::RemoveAbilityEndedModifiers()
{
	Super::RemoveAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Enemy_Status_EquippedWeapon);
}

FPlayMontageAndWaitParams UGA_EquipWeapons::GetPlayMontageParams() const
{
	FPlayMontageAndWaitParams PlayParams;
	PlayParams.PlayRate = FMath::FRandRange(MinRandomPlayRate, MaxRandomPlayRate);

	return PlayParams;
}

void UGA_EquipWeapons::OnAnimationFinished(bool bWasInterrupted)
{
	if (bWasInterrupted)
	{
		if (bForceEquippedStateOnCancel)
		{
			if (WeaponComp)
			{
				WeaponComp->PlayEquipAnimation();
			}
		}
	}

	TryApplyAbilityEndedModifiers();
}
