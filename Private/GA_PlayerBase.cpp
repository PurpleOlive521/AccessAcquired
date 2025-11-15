// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "GA_PlayerBase.h"

#include "PlayerCharacter.h"


UGA_PlayerBase::UGA_PlayerBase()
{
}

bool UGA_PlayerBase::CheckAbilityRequirements_Implementation() const
{
	const bool bParentSucceeded = Super::CheckAbilityRequirements_Implementation();

	if (!bParentSucceeded)
	{
		return false;
	}

	UGameplaySystemComponent* Component = GetOwningComponent_Checked();

	const bool bHasEnergy = Component->GetAttributeValue(EAttributeType::EAT_Energy) >= EnergyCost;

	return bHasEnergy;
}

bool UGA_PlayerBase::ApplyAbilityRequirements_Implementation()
{
	const bool bParentSucceeded = Super::ApplyAbilityRequirements_Implementation();

	if (!bParentSucceeded)
	{
		return false;
	}

	AActor* Owner = GetOwningActor();

	// Only able to be performed on the Player
	APlayerCharacter* Player = CastChecked<APlayerCharacter>(Owner); 

	return Player->RequestEnergyUse(EnergyCost);
}

FString UGA_PlayerBase::ToString() const
{
	const FString ParentString = Super::ToString();

	FString DisplayInfo = ParentString + FString::Printf(TEXT(" EnergyCost : %.2f"), EnergyCost);

	return DisplayInfo;
}

FString UGA_PlayerBase::ToStringWithDebugTags() const
{
	const FString ParentString = Super::ToStringWithDebugTags();

	FString DisplayInfo = ParentString + FString::Printf(TEXT(" EnergyCost : %.2f"), EnergyCost);

	return DisplayInfo;
}