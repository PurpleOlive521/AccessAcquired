// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#include "UnlockableSkillSystem.h"

#include "DevCommons.h"
#include "GameplayPersistenceSubsystem.h"
#include "GameplaySaveGame.h"
#include "AASaveGame.h"
#include "Kismet/GameplayStatics.h"

UUnlockableSkill* UUnlockableSkillSystem::GetSkillInstance(const TSubclassOf<UUnlockableSkill> SkillClass)
{
	const TObjectPtr<UUnlockableSkill>* SkillInstance = SkillInstanceTable.Find(SkillClass);
	if (!SkillInstance)
	{
		return nullptr;
	}

    return *SkillInstance;
}

void UUnlockableSkillSystem::InstantiateSkills()
{
	for (TSubclassOf<UUnlockableSkill> SkillClass : UnlockableSkills)
	{
		UUnlockableSkill* SkillInstance = NewObject<UUnlockableSkill>(this, SkillClass);
		if (SkillInstance)
		{
			SkillInstance->SetOwner(this);
			SkillInstanceTable.Add(SkillClass, SkillInstance);
		}
	}
}

void UUnlockableSkillSystem::QueryForUnlockableSkills(TArray<UUnlockableSkill*>& OutUnlockableSkills) const
{
	OutUnlockableSkills.Empty();

	// Iterate and get all currently unlockable skills
	for (const auto& UnlockableSkillPair : SkillInstanceTable)
	{
		if (UnlockableSkillPair.Value->IsUnlockable())
		{
			OutUnlockableSkills.Add(UnlockableSkillPair.Value);
		}
	}

	return;
}

bool UUnlockableSkillSystem::TryUnlockSkill(TSubclassOf<UUnlockableSkill> SkillClass)
{
	UUnlockableSkill* SkillInstance = GetSkillInstance(SkillClass);

	if (!SkillInstance)
	{
		UE_LOG(VSLog, Error, TEXT("UnlockableSkillSystem: No SkillInstance found"));
		return false;
	}
	
	bool bResult = SkillInstance->TryUnlockSkill();

	if (bResult == true)
	{
		OnSkillChangedDelegate.Broadcast();
	}

	return bResult;
}

void UUnlockableSkillSystem::Init()
{
	InstantiateSkills();

	UGameplayPersistenceSubsystem* GameplayPersistance = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UGameplayPersistenceSubsystem>();
	GameplayPersistance->RegisterAndLoadObject(this);
}

void UUnlockableSkillSystem::SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);

	// Save all instances, updating whether they are unlocked or not
	for (const TSubclassOf<UUnlockableSkill> Skill : UnlockableSkills)
	{
		TObjectPtr<UUnlockableSkill>* SkillInstancePtr = SkillInstanceTable.Find(Skill);
		
		if (!SkillInstancePtr)
		{
			UE_LOG(VSLog, Error, TEXT("No matching SkillInstance found in SkillInstanceTable when saving data."));
			return;
		}
		
		TObjectPtr<UUnlockableSkill> SkillInstance = *SkillInstancePtr;

		AASaveGameObject->SkillTreeSaveObject.UnlockedSkillTable.Add(SkillInstance->GetId(), SkillInstance->IsUnlocked());
	}
}

void UUnlockableSkillSystem::LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);

	// Find the TSubclass for each saved GUID and update the unlock status on the mapped instance
	for (const auto& SavedSkill : AASaveGameObject->SkillTreeSaveObject.UnlockedSkillTable)
	{
		for (const TSubclassOf<UUnlockableSkill> UnlockableSkill : UnlockableSkills)
		{
			// Does it have the same GUID as the saved one?
			if (UnlockableSkill->GetDefaultObject<UUnlockableSkill>()->GetId() == SavedSkill.Key)
			{
				TObjectPtr<UUnlockableSkill>* SkillInstancePtr = SkillInstanceTable.Find(UnlockableSkill);

				if (!SkillInstancePtr)
				{
					UE_LOG(VSLog, Error, TEXT("No matching SkillInstance found in SkillInstanceTable when loading data."));
					return;
				}

				TObjectPtr<UUnlockableSkill> SkillInstance = *SkillInstancePtr;

				SkillInstance->SetUnlockState(SavedSkill.Value);
			}
		}

	}
}
