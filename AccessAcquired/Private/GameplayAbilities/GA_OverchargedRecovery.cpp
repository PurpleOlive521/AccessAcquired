// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayAbilities/GA_OverchargedRecovery.h"

#include "GameplayTagDefines.h"
#include "EnemyAIControllerBase.h"
#include "GameplaySystemComponent.h"

UDEPRECATED_GA_OverchargedRecovery::UDEPRECATED_GA_OverchargedRecovery()
{
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_Overcharge);

	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_Overcharge);

	InstancingPolicy = EInstancingPolicy::EIP_InstancedPerActor;

	bIsCancellable = true;
}

void UDEPRECATED_GA_OverchargedRecovery::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility(ActivationData, OutActiveGameplayAbility); // Assigns properties from GA_AIActionBase

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

}

void UDEPRECATED_GA_OverchargedRecovery::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
}

void UDEPRECATED_GA_OverchargedRecovery::RemoveAbilityEndedModifiers()
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
