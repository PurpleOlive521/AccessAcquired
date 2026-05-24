// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UnlockableSkill.generated.h"

class UUnlockableSkillSystem;

UENUM(BlueprintType)
enum class ENodeType : uint8
{
	NT_Minor			UMETA(DisplayName = "Patch"),
	NT_Major			UMETA(DisplayName = "Update"),
};

UENUM(BlueprintType)
enum class ENodeState : uint8
{
	// No skill is assigned to the slot.
	ENS_Empty		UMETA(DisplayName = "Empty"),

	// The skill is not reachable from the currently unlocked skills.
	ENS_Locked		UMETA(DisplayName = "Locked"),

	// Skill unlock requirements are met and is in reach.
	ENS_Unlockable	UMETA(DisplayName = "Unlockable"),

	// The skill is reachable but the unlock requirements are not met.
	ENS_Blocked		UMETA(DisplayName = "Blocked"),

	// The skill is already unlocked.
	ENS_Unlocked	UMETA(DisplayName = "Unlocked"),
};

// An skill that can be unlocked by using resources of some kind, giving permanent buffs and effects when gained. 
UCLASS(Blueprintable)
class VERTICALSLICE_API UUnlockableSkill : public UObject
{
	GENERATED_BODY()

public:

	virtual UWorld* GetWorld() const override;

	// Tests against all requirements.
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "WorldContext"), Category = "UnlockableSkill")
	ENodeState IsUnlockable(const UObject* WorldContext) const;

	// Attempts to unlock the skill. If successfull, will take any required resources and apply any effects.
	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "WorldContext"), Category = "UnlockableSkill")
	bool TryUnlockSkill(const UObject* WorldContext) const;

	// Applies the unlock buffs and effects. Override to create your own unique skill.
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "On Skill Unlocked"), Category = "UnlockableSkill")
	void K2_OnSkillUnlocked() const;

	// Generates a custom display text, if any is specified by the skill. Override to create your own unique display text.
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "Get Custom Descriptor", DefaultToSelf = "WorldContext"), Category = "UnlockableSkill")
	FText K2_GetCustomDescriptor(const UObject* WorldContext, bool IsUnlocked);

protected:

	// Style to use when displaying & name to use
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillNode")
	ENodeType NodeType = ENodeType::NT_Minor;

	// The description of the effects gained when unlocked. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine = "True"), Category = "UnlockableSkill")
	FString Description = "";

	// --- Unlock requirements

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UnlockableSkill")
	int IntelCost = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UnlockableSkill")
	int RequiredLevel = 0;

	// --- Unlock conditions

	// Need to be unlocked before this one can be unlocked
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UnlockableSkill")
	TArray<TSubclassOf<UUnlockableSkill>> PrerequiredSkills;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UTexture2D> Icon;
};
