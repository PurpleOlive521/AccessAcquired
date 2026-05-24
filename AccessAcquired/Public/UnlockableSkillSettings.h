// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UnlockableSkillSettings.generated.h"

class UUnlockableSkill;

/**
 * Unlockable Skill settings.
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Unlockable Skill Settings"))
class VERTICALSLICE_API UUnlockableSkillSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "UnlockableSkillSettings")
	TArray<TSubclassOf<UUnlockableSkill>> GetAllSkills() const;

	UPROPERTY(EditAnywhere, Config, Category = "UnlockableSkillSettings")
	TSet<TSoftClassPtr<UUnlockableSkill>> AvailableSkills;
};