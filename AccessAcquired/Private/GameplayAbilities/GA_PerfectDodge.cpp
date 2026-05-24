// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayAbilities/GA_PerfectDodge.h"

#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"


UGA_PerfectDodge::UGA_PerfectDodge()
{
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_ModeSwitch);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);

	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_BlockingAction);
}

void UGA_PerfectDodge::ActivateAbility(const FGameplayAbilityActivationData& ActivationData, FActiveGameplayAbility& OutActiveGameplayAbility)
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	FGameplayTagSystem* TagSystem = GameplaySystem->GetGameplayTagSystem();
	TagSystem->AddTag(GAMEPLAYTAG_Status_Untargetable);
}

void UGA_PerfectDodge::ApplyAbilityEndedModifiers()
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	FGameplayTagSystem* TagSystem = GameplaySystem->GetGameplayTagSystem();
	TagSystem->RemoveTag(GAMEPLAYTAG_Status_Untargetable);
}

void UGA_PerfectDodge::RemoveAbilityEndedModifiers()
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	FGameplayTagSystem* TagSystem = GameplaySystem->GetGameplayTagSystem();
	TagSystem->AddTag(GAMEPLAYTAG_Status_Untargetable);
}
