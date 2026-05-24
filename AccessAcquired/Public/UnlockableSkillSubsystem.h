// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "UnlockableSkill.h"
#include "SaveableObjectInterface.h"

#include "UnlockableSkillSubsystem.generated.h"

class UGameplaySaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillChangedSignature, TSubclassOf<UUnlockableSkill>, SkillClass);

UCLASS()
class VERTICALSLICE_API UUnlockableSkillSubsystem : public UWorldSubsystem, public ISaveableObjectInterface
{
	GENERATED_BODY()

public:

	bool OnSerialize(FSaveGameArchive& Archive, bool bIsLoading);

	// Helper getter
	static UUnlockableSkillSubsystem* Get(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UnlockableSkillSystem")
	const UUnlockableSkill* GetSkillInstance(TSubclassOf<UUnlockableSkill> SkillClass) const;

	// Returns true if any skill is currently able to be unlocked.
	UFUNCTION(BlueprintCallable, Category = "UnlockableSkillSystem")
	bool HasUnlockableSkills() const;

	// Lets other nodes know of the change, so they can recheck their unlock requirements. Returns true if successful.
	UFUNCTION(BlueprintCallable, Category = "UnlockableSkillSystem")
	bool TryUnlockSkill(TSubclassOf<UUnlockableSkill> SkillClass);
	
	UFUNCTION(BlueprintCallable, Category = "UnlockableSkillSystem")
	bool IsSkillUnlocked(TSubclassOf<UUnlockableSkill> SkillClass) const;

	UFUNCTION(BlueprintCallable, Category = "UnlockableSkillSystem")
	ENodeState GetSkillState(TSubclassOf<UUnlockableSkill> SkillClass) const;

protected:

	UPROPERTY(SaveGame)
	TMap<TSubclassOf<UUnlockableSkill>, bool> UnlockedSkills;

public:

	// --- Delegates
	UPROPERTY(BlueprintAssignable)
	FOnSkillChangedSignature OnSkillChangedDelegate;
};
