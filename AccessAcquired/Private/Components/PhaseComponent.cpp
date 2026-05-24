// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Components/PhaseComponent.h"
#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "GameplaySystemBlueprintLibrary.h"

UPhaseComponent::UPhaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPhaseComponent::BeginPlay()
{
	Super::BeginPlay();

	UGameplaySystemComponent* DerefGameplaySystem = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(GetOwner());
	if (DerefGameplaySystem)
	{
		GameplaySystem = MakeWeakObjectPtr(DerefGameplaySystem);
		DerefGameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_Health).AddUObject(this, &UPhaseComponent::ListenForHealth);
		DerefGameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_MaxHealth).AddUObject(this, &UPhaseComponent::ListenForHealth);

		FGameplayTagSystem& GameplayTagSystem = DerefGameplaySystem->GetGameplayTagSystemAsRef();
		GameplayTagSystem.OnGameplayTagModifiedDelegate.AddUObject(this, &UPhaseComponent::ListenForGameplayTags);
	}
}

#if WITH_EDITORONLY_DATA

void UPhaseComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	PhaseThresholds.Sort([](const FPhaseThreshold& A, const FPhaseThreshold& B)
		{
			return A.Phase < B.Phase;
		});
}

#endif // WITH_EDITORONLY_DATA

void UPhaseComponent::SetPhase(int32 Phase)
{
	if (Phase <= CurrentPhase)
	{
		SetPhase_Internal(Phase);
		return;
	}

	// Walk to new phase so that we invoke the delegate for each phase
	for (int32 i = CurrentPhase + 1; i <= Phase; i++)
	{
		SetPhase_Internal(i);
	}
}

void UPhaseComponent::ListenForGameplayTags(FGameplayTag GameplayTag, int NewCount, int Delta)
{
	if (GameplayTag == GAMEPLAYTAG_Enemy_Status_PhaseCount)
	{
		if (NewCount >= 0)
		{
			// PhaseCount represents phase n + 1
			SetPhase(NewCount + 1);
		}
	}
}

void UPhaseComponent::ListenForHealth(EAttributeType ChangedAttribute)
{
	ensure(GameplaySystem.IsValid());

	const float HealthPercentage = 100.0f * UGameplaySystemBlueprintLibrary::GetHealthAsPercentage(GameplaySystem.Get());

	for (auto PhaseThreshold : PhaseThresholds)
	{
		if (PhaseThreshold.Phase <= CurrentPhase)
		{
			continue;
		}

		if (PhaseThreshold.Health >= HealthPercentage)
		{
			PhaseTriggered(PhaseThreshold);
		}
	}
}

void UPhaseComponent::SetPhase_Internal(int32 Phase)
{
	if (CurrentPhase == Phase)
	{
		return;
	}

	CurrentPhase = Phase;

	OnChangePhaseDelegate.Broadcast(CurrentPhase);
}

void UPhaseComponent::PhaseTriggered(FPhaseThreshold& Threshold)
{
	SetPhase(Threshold.Phase);
}
