// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayAbilities/GA_Overcharged.h"

#include "GameplayTagDefines.h"
#include "EnemyAIControllerBase.h"
#include "GameplaySystemComponent.h"

UGA_Overcharged::UGA_Overcharged()
{
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_Overcharge);
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	InstancingPolicy = EInstancingPolicy::EIP_InstancedPerActor;
	
	bIsCancellable = true;
}

void UGA_Overcharged::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility(ActivationData, OutActiveGameplayAbility);

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);

	// Set AI State
	if (APawn* Pawn = Cast<APawn>(GetOwningActor_Checked()))
	{
		if (AEnemyAIControllerBase* AIController = Cast<AEnemyAIControllerBase>(Pawn->GetController()))
		{
			FrozenState = AIController->PushState<UAIStateLayer_Frozen>();
		}
	}
	
	// Animation should be handled in Blueprint with GameplayTasks
}

void UGA_Overcharged::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UGA_Overcharged::RemoveAbilityEndedModifiers()
{
	Super::RemoveAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UGA_Overcharged::EndAbility()
{
	if (APawn* Pawn = Cast<APawn>(GetOwningActor_Checked()))
	{
		if (AEnemyAIControllerBase* AIController = Cast<AEnemyAIControllerBase>(Pawn->GetController()))
		{
			AIController->RemoveState(FrozenState);
		}
	}

	Super::EndAbility();
}

void UGA_Overcharged::CancelAbility()
{
	if (APawn* Pawn = Cast<APawn>(GetOwningActor_Checked()))
	{
		if (AEnemyAIControllerBase* AIController = Cast<AEnemyAIControllerBase>(Pawn->GetController()))
		{
			AIController->RemoveState(FrozenState);
		}
	}

	Super::CancelAbility();
}