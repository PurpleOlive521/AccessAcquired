// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "OverheatComponent.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "DevCommons.h"
#include "PlayerCharacter.h"

UOverheatComponent::UOverheatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ModeStates.Add(EDamageType::EDT_Infrared);
	ModeStates.Add(EDamageType::EDT_Ultraviolet);
	ModeStates.Add(EDamageType::EDT_Gamma);
}

void UOverheatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Player = Cast<APlayerCharacter>(GetOwner());

	if (IGameplaySystemOwnerInterface* GameplaySystemInterface = Cast<IGameplaySystemOwnerInterface>(GetOwner()))
	{
		GameplaySystem = GameplaySystemInterface->Execute_GetGameplaySystemComponent(Player.Get());
		check(GameplaySystem.IsValid());

		TArray<FOnAttributeChangedSignature*> OutDelegates;
		GameplaySystem->OnAttributeChangedDelegateCollection.GetMultipleDelegates(AttributeGroups::OverheatAttributes, OutDelegates);
		for (auto& Delegate : OutDelegates)
		{
			Delegate->AddUObject(this, &UOverheatComponent::ListenForOverheat);
		}

		GameplaySystem->GetGameplayTagSystem()->OnGameplayTagChangedDelegate.AddUObject(this, &UOverheatComponent::ListenForGameplayTags);
	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("OverheatComponent: Could not find GameplaySystemComponent in Owner!"));
		return;
	}
}

void UOverheatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UOverheatComponent::ListenForOverheat(EAttributeType ChangedAttribute)
{
	UpdateOverheat(ChangedAttribute);
	UpdateSeverity(ChangedAttribute);
	UpdateOverclock(ChangedAttribute);
}

void UOverheatComponent::ListenForGameplayTags(FGameplayTag ModifiedTag, bool bWasAdded)
{
	if (ModifiedTag == GAMEPLAYTAG_Status_Overclocking)
	{
		if (bWasAdded)
		{
			OnOverclockedDelegate.Broadcast(true);
		}
		else
		{
			OnOverclockedDelegate.Broadcast(false);
		}
	}
}

void UOverheatComponent::UpdateOverheat(EAttributeType OverheatType)
{
	const float Value = GameplaySystem->GetAttributeValue(OverheatType, EAttributeValue::EAV_CurrentValue);
	const float MaxValue = GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverheatLimit, EAttributeValue::EAV_CurrentValue);

	const EDamageType Mode = UGameplayUtilityBlueprintLibrary::GetCorrespondingMode(OverheatType);
	FModeState& State = ModeStates.FindChecked(Mode);

	if (Value >= MaxValue)
	{
		if (State.bIsOverheating)
		{
			return;
		}

		State.bIsOverheating = true;
		GameplaySystem->AddTag(GAMEPLAYTAG_Status_Overheating);

		OnModeOverheatedDelegate.Broadcast(true);
	}
	else
	{
		if (!State.bIsOverheating)
		{
			return;
		}

		State.bIsOverheating = false;
		GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_Overheating);

		OnModeOverheatedDelegate.Broadcast(false);
	}
}

void UOverheatComponent::UpdateSeverity(EAttributeType OverheatType)
{
	const float Value = GameplaySystem->GetAttributeValue(OverheatType, EAttributeValue::EAV_CurrentValue);
	const float MaxValue = GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverheatLimit, EAttributeValue::EAV_CurrentValue);

	const float OverclockThreshold = GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverclockThreshold, EAttributeValue::EAV_CurrentValue);
	const float OverclockStart = 0.01f * MaxValue * OverclockThreshold;

	const EDamageType Mode = UGameplayUtilityBlueprintLibrary::GetCorrespondingMode(OverheatType);

	if (Value < OverclockStart)
	{
		SetSeverity(Mode, EOverheatSeverity::EOS_None);
		return;
	}

	const float RelativeOverheat = (Value - OverclockStart) / fabs(100.0f - OverclockThreshold);

	if (RelativeOverheat < OVERHEAT_SEVERITY_MILD)
	{
		SetSeverity(Mode, EOverheatSeverity::EOS_Mild);
		return;
	}

	if (RelativeOverheat < OVERHEAT_SEVERITY_MODERATE)
	{
		SetSeverity(Mode, EOverheatSeverity::EOS_Moderate);
		return;
	}

	if (RelativeOverheat < OVERHEAT_SEVERITY_SEVERE)
	{
		SetSeverity(Mode, EOverheatSeverity::EOS_Severe);
		return;
	}

	if (RelativeOverheat < OVERHEAT_SEVERITY_CRITICAL)
	{
		SetSeverity(Mode, EOverheatSeverity::EOS_Critical);
		return;
	}
}

void UOverheatComponent::UpdateOverclock(EAttributeType OverheatType)
{
	const float Value = GameplaySystem->GetAttributeValue(OverheatType, EAttributeValue::EAV_CurrentValue);
	const float MaxValue = GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverheatLimit, EAttributeValue::EAV_CurrentValue);

	const float OverclockThreshold = 0.01f * GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverclockThreshold, EAttributeValue::EAV_CurrentValue);
	const float OverclockStart = MaxValue * OverclockThreshold;

	const EDamageType Mode = UGameplayUtilityBlueprintLibrary::GetCorrespondingMode(OverheatType);
	FModeState& State = ModeStates.FindChecked(Mode);

	if (Value < OverclockStart)
	{
		if (!State.bIsOverclocking)
		{
			return;
		}

		State.bIsOverclocking = false;
		GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_Overclocking);
	}
	else
	{
		if (State.bIsOverclocking)
		{
			return;
		}

		State.bIsOverclocking = true;
		GameplaySystem->AddTag(GAMEPLAYTAG_Status_Overclocking);
	}
}

void UOverheatComponent::SetSeverity(EDamageType Mode, EOverheatSeverity NewSeverity)
{
	FModeState& State = ModeStates.FindChecked(Mode);

	if (State.Severity == NewSeverity)
	{
		return;
	}

	State.Severity = NewSeverity;

	const EOverheatSeverity HighestSeverity = GetHighestSeverity();
	if (ActiveSeverity != HighestSeverity)
	{
		ActiveSeverity = HighestSeverity;
		OnChangeSeverityDelegate.Broadcast(ActiveSeverity);
	}
}

EOverheatSeverity UOverheatComponent::GetHighestSeverity()
{
	EOverheatSeverity Highest = EOverheatSeverity::EOS_None;
	for (const auto& [Mode, State] : ModeStates)
	{
		if (State.Severity > Highest)
		{
			Highest = State.Severity;
		}
	}

	return Highest;
}

