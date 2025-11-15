// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

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


// An skill that can be unlocked by using resources of some kind, giving permanent buffs and effects when gained. 
UCLASS(Blueprintable)
class VERTICALSLICE_API UUnlockableSkill : public UObject
{
	GENERATED_BODY()

public:
	
	// Copied from GameplayEffect.h, maybe move to DevCommons since its a static function already?
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	static void GenerateGUID(FString& IdRef);

	virtual UWorld* GetWorld() const override;

	// --- Unlockable Skill

	// Tests against all requirements. Returns true if its unlockable, false otherwise.
	UFUNCTION(BlueprintCallable, Category = "UnlockableSkill")
	bool IsUnlockable() const;

	// Attempts to unlock the skill. If successfull, will take any required resources and apply any effects.
	UFUNCTION(BlueprintCallable, Category = "UnlockableSkill")
	bool TryUnlockSkill();

	// Applies the unlock buffs and effects. Override to create your own unique skill.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UnlockableSkill")
	void OnSkillUnlocked();
	virtual void OnSkillUnlocked_Implementation();

	// Generates a custom display text, if any is specified by the skill. Override to create your own unique display text.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UnlockableSkill")
	FString GetCustomDescriptor();
	virtual FString GetCustomDescriptor_Implementation();

	UFUNCTION(BlueprintCallable, Category = "UnlockableSkill")
	bool IsUnlocked() const;

	UFUNCTION(BlueprintCallable, Category = "UnlockableSkill")
	void SetOwner(class UUnlockableSkillSystem* Owner);

	UFUNCTION(BlueprintCallable, Category = "UnlockableSkill")
	FString GetId();

	// Updates IsUnlocked internally without performing any costs or calling OnSkillUnlock.
	UFUNCTION(BlueprintCallable, Category = "UnlockableSkill")
	void SetUnlockState(bool State);

private:

	UPROPERTY()
	TWeakObjectPtr<UUnlockableSkillSystem> SkillSystem;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UnlockableSkill")
	bool bIsUnlocked = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UTexture2D> Icon;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility", meta = (EditCondition = "false"))
	FString Id = "";

};
