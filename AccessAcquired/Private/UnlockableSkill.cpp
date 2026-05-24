// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "UnlockableSkill.h"

#include "UnlockableSkillSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameplaySystemComponent.h"
#include "GameCurrencySubsystem.h"
#include "DevCommons.h"

UWorld* UUnlockableSkill::GetWorld() const
{
	if (HasAllFlags(RF_ClassDefaultObject) == false)
	{
		// Get World from Outer
		return GetOuter()->GetWorld();
	}

	// Return nullptr if we are CDO.
	return nullptr;
}

ENodeState UUnlockableSkill::IsUnlockable(const UObject* WorldContext) const
{
	check(WorldContext);

	UUnlockableSkillSubsystem* SkillSubsystem = UUnlockableSkillSubsystem::Get(WorldContext);
	check(SkillSubsystem);

	// Allows it to be clicked in the UI while already unlocked
	if (SkillSubsystem->IsSkillUnlocked(GetClass()))
	{
		return ENodeState::ENS_Unlocked;
	}

	UGameCurrencySubsystem* GameCurrencySystem = UGameCurrencySubsystem::Get(WorldContext);
	if (GameCurrencySystem == nullptr)
	{
		UE_LOG(VSLog, Error, TEXT("GameplayPersistanceSubsystem returned null. Could not check if skill is unlockable."))
		return ENodeState::ENS_Locked;
	}

	// Make sure each prerequisite skill is unlocked
	for (const TSubclassOf<UUnlockableSkill>& Skill : PrerequiredSkills)
	{
		if (not SkillSubsystem->IsSkillUnlocked(Skill))
		{
			return ENodeState::ENS_Locked;
		}
	}

	// Check currency requirements
	int CurrentIntel = GameCurrencySystem->GetIntel();
	if (CurrentIntel < IntelCost)
	{
		return ENodeState::ENS_Blocked;
	}

	UGameplaySystemComponent* GameplaySystem = UGameplaySystemComponent::GetGameplaySystemFromActor(UGameplayStatics::GetPlayerCharacter(WorldContext, 0));
	if (!GameplaySystem)
	{
		UE_LOG(VSLog, Error, TEXT("UnlockableSkill: No GameplaySystemComponent found"));
		return ENodeState::ENS_Locked;
	}

	// Check level requirements
	if (GameplaySystem->GetEntityLevel() < RequiredLevel)
	{
		return ENodeState::ENS_Blocked;
	}

	return ENodeState::ENS_Unlockable;
}

bool UUnlockableSkill::TryUnlockSkill(const UObject* WorldContext) const
{
	if (IsUnlockable(WorldContext) != ENodeState::ENS_Unlockable)
	{
		return false;
	}

	// Execute all costs for unlocking the ability

	UGameCurrencySubsystem* GameCurrencySystem = UGameCurrencySubsystem::Get(WorldContext);
	if (!GameCurrencySystem)
	{
		UE_LOG(VSLog, Error, TEXT("GameplayPersistanceSubsystem returned null. Could not check if skill is unlockable."))
		return false;
	}

	GameCurrencySystem->ChangeIntel(-IntelCost, true);
	
	K2_OnSkillUnlocked();

	return true;
}