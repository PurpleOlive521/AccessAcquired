// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "UnlockableSkill.h"

#include "UnlockableSkillSystem.h"
#include "Kismet/GameplayStatics.h"
#include "GameplaySystemComponent.h"
#include "GameCurrencySubsystem.h"
#include "GameplaySystemOwnerInterface.h"
#include "DevCommons.h"

#if WITH_EDITOR
void UUnlockableSkill::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	GenerateGUID(Id);
}
#endif

void UUnlockableSkill::GenerateGUID(FString& IdRef)
{
	bool bHasIdAlready = IdRef != "";

	if (bHasIdAlready)
	{
		return;
	}

	IdRef = FGuid::NewGuid().ToString();
}

UWorld* UUnlockableSkill::GetWorld() const
{
	// Similar implementation to Unreals own GameplayAbility. Uses the Outer to get World, which in our case is the PlayerController

	if (HasAllFlags(RF_ClassDefaultObject) == false)
	{
		// Get World from Outer
		return GetOuter()->GetWorld();
	}

	// Return nullptr if we are CDO.
	return nullptr;
}

bool UUnlockableSkill::IsUnlockable() const
{
	// Allows it to be clicked in the UI while already unlocked
	if (IsUnlocked() == true)
	{
		return false;
	}

	UGameCurrencySubsystem* GameCurrencySystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameCurrencySubsystem>();
	if (GameCurrencySystem == nullptr)
	{
		UE_LOG(VSLog, Error, TEXT("GameplayPersistanceSubsystem returned null. Could not check if skill is unlockable."))
		return false;
	}

	// Check currency requirements
	int CurrentIntel = GameCurrencySystem->GetIntel();
	if (CurrentIntel < IntelCost)
	{
		return false;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UGameplaySystemComponent* GameplaySystem = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(PlayerController->GetPawn());
	if (!GameplaySystem)
	{
		UE_LOG(VSLog, Error, TEXT("UnlockableSkill: No GameplaySystemComponent found"));
		return false;
	}

	// Check level requirements
	if (GameplaySystem->GetEntityLevel() < RequiredLevel)
	{
		return false;
	}
	
	// Make sure each prerequisite skill is unlocked
	for (const TSubclassOf<UUnlockableSkill>& Skill : PrerequiredSkills)
	{
		UUnlockableSkill* SkillInstance = SkillSystem->GetSkillInstance(Skill);

		if (!SkillInstance)
		{
			return false;
		}

		if (SkillInstance->IsUnlocked() == false)
		{
			return false;
		}
	}

	return true;
}

bool UUnlockableSkill::TryUnlockSkill()
{
	if (IsUnlockable() == false)
	{
		return false;
	}

	// Execute all costs for unlocking the ability

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController)
	{
		UE_LOG(VSLog, Error, TEXT("UnlockableSkill: No PlayerController found"));
		return false;
	}

	UGameCurrencySubsystem* GameCurrencySystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameCurrencySubsystem>();
	if (GameCurrencySystem == nullptr)
	{
		UE_LOG(VSLog, Error, TEXT("GameplayPersistanceSubsystem returned null. Could not check if skill is unlockable."))
		return false;
	}

	GameCurrencySystem->ChangeIntel(-IntelCost, true);
	
	bIsUnlocked = true;

	OnSkillUnlocked();

	return true;
}

void UUnlockableSkill::OnSkillUnlocked_Implementation()
{
}

FString UUnlockableSkill::GetCustomDescriptor_Implementation()
{
	return "";
}

bool UUnlockableSkill::IsUnlocked() const
{
	return bIsUnlocked;
}

void UUnlockableSkill::SetOwner(UUnlockableSkillSystem* Owner)
{
	SkillSystem = Owner;
}

FString UUnlockableSkill::GetId()
{
	return Id;
}

void UUnlockableSkill::SetUnlockState(bool State)
{
	bIsUnlocked = State;
}

