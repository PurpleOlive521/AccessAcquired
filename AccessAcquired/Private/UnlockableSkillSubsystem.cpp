// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "UnlockableSkillSubsystem.h"

#include "DevCommons.h"
#include "GameplayPersistenceSubsystem.h"
#include "GameplaySaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UnlockableSkillSettings.h"

bool UUnlockableSkillSubsystem::OnSerialize(FSaveGameArchive& Archive, bool bIsLoading)
{
	const bool bSuccess = Archive.SerializeField(TEXT("UnlockableSKillSubsystem"), [&](FStructuredArchive::FSlot Slot)
		{
			SerializeScriptProperties(Slot);
		});

	return bSuccess;
}

UUnlockableSkillSubsystem* UUnlockableSkillSubsystem::Get(const UObject* WorldContext)
{
	if (WorldContext)
	{
		if (const UWorld* World = WorldContext->GetWorld())
		{
			UUnlockableSkillSubsystem* SkillSubsystem = World->GetSubsystem<UUnlockableSkillSubsystem>();
			ensure(SkillSubsystem);

			return SkillSubsystem;
		}
	}
	
	return nullptr;
}

const UUnlockableSkill* UUnlockableSkillSubsystem::GetSkillInstance(const TSubclassOf<UUnlockableSkill> SkillClass) const
{
	if (SkillClass)
	{
		return SkillClass->GetDefaultObject<UUnlockableSkill>();
	}

    return nullptr;
}

bool UUnlockableSkillSubsystem::HasUnlockableSkills() const
{
	const UUnlockableSkillSettings* Settings = GetDefault<UUnlockableSkillSettings>();

	TArray<TSubclassOf<UUnlockableSkill>> AvailableSkills = Settings->GetAllSkills();

	for (auto SkillClass : AvailableSkills)
	{
		if (const UUnlockableSkill* SkillInstance = GetSkillInstance(SkillClass))
		{
			if (SkillInstance->IsUnlockable(this) == ENodeState::ENS_Unlockable)
			{
				return true;
			}
		}
	}

	return false;
}

bool UUnlockableSkillSubsystem::TryUnlockSkill(TSubclassOf<UUnlockableSkill> SkillClass)
{
	const UUnlockableSkill* SkillInstance = GetSkillInstance(SkillClass);

	if (!SkillInstance)
	{
		UE_LOG(VSLog, Error, TEXT("UnlockableSkillSystem: No SkillInstance found"));
		return false;
	}
	
	bool bResult = SkillInstance->TryUnlockSkill(this);

	if (bResult)
	{
		UnlockedSkills.Emplace(SkillClass, true);
		OnSkillChangedDelegate.Broadcast(SkillClass);
	}

	return bResult;
}

bool UUnlockableSkillSubsystem::IsSkillUnlocked(TSubclassOf<UUnlockableSkill> SkillClass) const
{
	return UnlockedSkills.FindRef(SkillClass);
}

ENodeState UUnlockableSkillSubsystem::GetSkillState(TSubclassOf<UUnlockableSkill> SkillClass) const
{	
	if (const UUnlockableSkill* SkillInstance = GetSkillInstance(SkillClass))
	{
		return SkillInstance->IsUnlockable(this);
	}
	else
	{
		return ENodeState::ENS_Empty;
	}
}
