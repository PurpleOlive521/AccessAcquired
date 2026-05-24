// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GA_ModeSwitch.h"

#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "ModeComponent.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "GameplaySystemBlueprintLibrary.h"
#include "DevCommons.h"
#include "DevelopmentTypes.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


UGA_ModeSwitch::UGA_ModeSwitch()
{
	ActivationBlockedTags.AddTag(GAMEPLAYTAG_Status_PerformingAction);
	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_Dash);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Status_ActionCancellable);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_ModeSwitch);
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_ModeSwitch);
	AbilityTags.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);
}

bool UGA_ModeSwitch::CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const
{
	FAIL_ON_FAILED_SUPER(CheckAbilityRequirements(ActivationData));

	check(GetOwningComponent());

	const FGameplaySystemActorInfo* ActorInfo = GetOwningComponent()->GetActorInfo();

	// Check that we have the mode available
	if (UModeComponent* ModeComponent = GetOwningActor_Checked()->GetComponentByClass<UModeComponent>())
	{
		if (!ModeComponent->HasMode(TargetMode))
		{
			return false;
		}
	}

	return true; 
}

bool UGA_ModeSwitch::ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData)
{
	FAIL_ON_FAILED_SUPER(ApplyAbilityRequirements(ActivationData));

	return true;
}

void UGA_ModeSwitch::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility(ActivationData, OutActiveGameplayAbility);

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_CanPerfectDodge);

	if (UModeComponent* ModeComponent = GetOwningActor_Checked()->GetComponentByClass<UModeComponent>())
	{
		ModeComponent->SetMode(TargetMode);
	}

	// Animation should be handled in Blueprint with GameplayTasks
}

void UGA_ModeSwitch::ApplyAbilityEndedModifiers()
{
	Super::ApplyAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_CanPerfectDodge);
}

void UGA_ModeSwitch::RemoveAbilityEndedModifiers()
{
	Super::RemoveAbilityEndedModifiers();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_CanPerfectDodge);
}