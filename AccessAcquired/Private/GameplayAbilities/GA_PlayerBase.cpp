// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GA_PlayerBase.h"

#include "PlayerCharacter.h"
#include "DevCommons.h"
#include "GameplayUtilityBlueprintLibrary.h"

UGA_PlayerBase::UGA_PlayerBase()
{
	OverheatCost = FMath::Abs(OverheatCost);
}

bool UGA_PlayerBase::CheckAbilityRequirements(const FGameplayAbilityActivationData& ActivationData) const
{
	const bool bParentSucceeded = Super::CheckAbilityRequirements(ActivationData);

	if (!bParentSucceeded)
	{
		return false;
	}

	UGameplaySystemComponent* Component = GetOwningComponent_Checked();

	AActor* Owner = GetOwningActor();
	UModeComponent* ModeComponent = Owner->GetComponentByClass<UModeComponent>();

	if (!ModeComponent)
	{
		UE_LOG(VSLog, Error, TEXT("GA_PlayerBase: Tried to use ability on character without a ModeComponent!"));
		return false;
	}

	EDamageType Mode = ModeComponent->GetMode();

	// No mode to source Overheat from.
	if (Mode == EDamageType::EDT_Normal)
	{
		if (OverheatCost > 0.0f)
		{
			return false;
		}

		// No cost associated, being in Normal mode doesn't matter.
		return true;
	}

	EAttributeType TargetOverheat = UGameplayUtilityBlueprintLibrary::GetCorrespondingOverheat(Mode);
	const bool bHasEnergy = Component->GetAttributeValue(TargetOverheat, EAttributeValue::EAV_CurrentValue) >= OverheatCost;

	return bHasEnergy;
}

bool UGA_PlayerBase::ApplyAbilityRequirements(const FGameplayAbilityActivationData& ActivationData)
{
	const bool bParentSucceeded = Super::ApplyAbilityRequirements(ActivationData);

	if (!bParentSucceeded)
	{
		return false;
	}

	AActor* Owner = GetOwningActor_Checked();
	UModeComponent* ModeComponent = Owner->GetComponentByClass<UModeComponent>();
	check(ModeComponent);

	EDamageType Mode = ModeComponent->GetMode();
	EAttributeType TargetOverheat = UGameplayUtilityBlueprintLibrary::GetCorrespondingOverheat(Mode);

	UGameplaySystemComponent* Component = GetOwningComponent_Checked();
	Component->ModifyAttributeValue(TargetOverheat, EAttributeValue::EAV_BaseValue, -OverheatCost);

	return true;
}

FString UGA_PlayerBase::ToString() const
{
	const FString ParentString = Super::ToString();

	FString DisplayInfo = ParentString + FString::Printf(TEXT(" Overheat Req. : %.2f"), OverheatCost);

	return DisplayInfo;
}

FString UGA_PlayerBase::ToStringWithDebugTags() const
{
	const FString ParentString = Super::ToStringWithDebugTags();

	FString DisplayInfo = ParentString + FString::Printf(TEXT(" Overheat Req. : %.2f"), OverheatCost);

	return DisplayInfo;
}