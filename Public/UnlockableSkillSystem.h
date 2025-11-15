// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "UnlockableSkill.h"
#include "SaveableObjectInterface.h"

#include "UnlockableSkillSystem.generated.h"

class UGameplaySaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillChangedSignature);

UCLASS(BlueprintType, Blueprintable)
class VERTICALSLICE_API UUnlockableSkillSystem : public UObject, public ISaveableObjectInterface
{
	GENERATED_BODY()
	
private:
	// Maps a unlockable skill to its instanced object
	UPROPERTY()
	TMap<TSubclassOf<UUnlockableSkill>, TObjectPtr<UUnlockableSkill>> SkillInstanceTable;

protected:

	// The skills available to the player. Required to be added here for it to be accessible in-game
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<TSubclassOf<UUnlockableSkill>> UnlockableSkills;

public:

	// --- Begin ISaveableObjectInterface
	void SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject) override;

	void LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject) override;
	
	// --- End ISaveableObjectInterface

	UFUNCTION(BlueprintCallable, Category = "UnlockableSkillSystem")
	UUnlockableSkill* GetSkillInstance(const TSubclassOf<UUnlockableSkill> SkillClass);

	// Creates all the instances to be used when working with the skills.
	UFUNCTION(BlueprintCallable, Category = "UnlockableSkillSystem")
	void InstantiateSkills();

	UFUNCTION(BlueprintCallable, Category = "UnlockableSkillSystem")
	void QueryForUnlockableSkills(TArray<UUnlockableSkill*>& OutUnlockableSkills) const;

	// Lets other nodes know of the change, so they can recheck their unlock requirements. Returns true if successful.
	UFUNCTION(BlueprintCallable, Category = "UnlockableSkillSystem")
	bool TryUnlockSkill(TSubclassOf<UUnlockableSkill> SkillClass);

	// We rely on the object owner to inform us when we can safely initialise.
	UFUNCTION(BlueprintCallable, Category = "UnlockableSkillSystem")
	void Init();

	// --- Delegates
	UPROPERTY(BlueprintAssignable)
	FOnSkillChangedSignature OnSkillChangedDelegate;

};
