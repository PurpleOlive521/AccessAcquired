// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "UnlockableSkillSettings.h"
#include "UnlockableSkill.h"

TArray<TSubclassOf<UUnlockableSkill>> UUnlockableSkillSettings::GetAllSkills() const
{
	TArray<TSubclassOf<UUnlockableSkill>> SkillClasses;

	for (auto SoftSkillClass : AvailableSkills)
	{
		if (TSubclassOf<UUnlockableSkill> SkillClass = SoftSkillClass.LoadSynchronous())
		{
			SkillClasses.Emplace(SkillClass);
		}
	}

	return SkillClasses;
}