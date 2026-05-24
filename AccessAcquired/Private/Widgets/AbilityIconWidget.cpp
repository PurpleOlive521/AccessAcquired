// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/AbilityIconWidget.h"

#include "GameplaySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CharacterBase.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "DevCommons.h"
#include "GA_PlayerBase.h"

void UAbilityIconWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	if (ACharacterBase* Character = Cast<ACharacterBase>(Player))
	{
		BoundCharacter = MakeWeakObjectPtr(Character);

		FGameplayAbilitySlotContainer* AbilitySlots = BoundCharacter->GetAbilitySlots();

		AbilityActivatedHandle = AbilitySlots->OnSlotAbilityActivatedDelegate.AddUObject(this, &UAbilityIconWidget::OnAbilityActivated);
		AbilityActivatedHandle = AbilitySlots->OnSlotAbilityFinishedDelegate.AddUObject(this, &UAbilityIconWidget::OnAbilityFinished);
	}

	if (UGameplaySystemComponent* GameplaySystem = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(Player))
	{
		BoundGameplaySystem = MakeWeakObjectPtr(GameplaySystem);
		FDelegateCollection& Delegates = BoundGameplaySystem->OnAttributeChangedDelegateCollection;

		OverheatInfraredHandle =	Delegates.GetDelegate(EAttributeType::EAT_InfraredOverheat).AddUObject(this, &UAbilityIconWidget::OnOverheatChanged);
		OverheatUltravioletHandle = Delegates.GetDelegate(EAttributeType::EAT_UltravioletOverheat).AddUObject(this, &UAbilityIconWidget::OnOverheatChanged);
		OverheatGammaHandle =		Delegates.GetDelegate(EAttributeType::EAT_GammaOverheat).AddUObject(this, &UAbilityIconWidget::OnOverheatChanged);
	}

	if (UModeComponent* ModeComponent = BoundCharacter->GetModeComponent())
	{
		BoundModeComponent = MakeWeakObjectPtr(ModeComponent);
		check(ModeComponent);

		ModeComponent->OnModeSwitchedDelegate.AddDynamic(this, &UAbilityIconWidget::OnModeSwitched);

		OnModeSwitched(ModeComponent->GetMode());
	}

	K2_OnStateChanged();
}

void UAbilityIconWidget::NativeDestruct()
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (ACharacterBase* DerefCharacter = BoundCharacter.Get())
	{
		FGameplayAbilitySlotContainer* AbilitySlots = DerefCharacter->GetAbilitySlots();

		AbilitySlots->OnSlotAbilityActivatedDelegate.Remove(AbilityActivatedHandle);
		AbilitySlots->OnSlotAbilityFinishedDelegate.Remove(AbilityFinishedHandle);
	}

	if (UModeComponent* DerefModeComponent = BoundModeComponent.Get())
	{
		DerefModeComponent->OnModeSwitchedDelegate.RemoveAll(this);
	}

	if (UGameplaySystemComponent* DerefGameplaySystem = BoundGameplaySystem.Get())
	{
		FDelegateCollection& Delegates = BoundGameplaySystem->OnAttributeChangedDelegateCollection;

		Delegates.GetDelegate(EAttributeType::EAT_InfraredOverheat).Remove(OverheatInfraredHandle);
		Delegates.GetDelegate(EAttributeType::EAT_UltravioletOverheat).Remove(OverheatUltravioletHandle);
		Delegates.GetDelegate(EAttributeType::EAT_GammaOverheat).Remove(OverheatGammaHandle);
	}

	Super::NativeDestruct();
}

void UAbilityIconWidget::OnAbilityActivated(TSubclassOf<UGameplayAbility> ActivatedAbility, const FGameplayTag& SlotTag)
{
	// Another ability might activate from the same GameplayAbilitySlotContainer
	if (not TargetSlot || SlotTag != TargetSlot->SlotTag)
	{
		return;
	}

	SetState(EAbilityState::EAS_Active);
}

void UAbilityIconWidget::OnAbilityFinished(TSubclassOf<UGameplayAbility> ActivatedAbility, const FGameplayTag& SlotTag)
{
	// Another ability might finish from the same GameplayAbilitySlotContainer
	if (not TargetSlot || SlotTag != TargetSlot->SlotTag)
	{
		return;
	}

	SetState(EAbilityState::EAS_NotReady);
}

void UAbilityIconWidget::OnModeSwitched(EDamageType Mode)
{
	if (Mode == CurrentMode)
	{
		return;
	}

	CurrentMode = Mode;

	FGameplayTag SlotTag = UGameplayUtilityBlueprintLibrary::GetTagChild(GAMEPLAYTAG_AbilitySlot_Ability, CurrentMode);
	FGameplayAbilitySlotContainer* AbilitySlots = BoundCharacter.Get()->GetAbilitySlots();
	TargetSlot = AbilitySlots->GetSlot(SlotTag);

	TSubclassOf<UGameplayAbility> StoredAbility = TargetSlot->Ability;

	if(StoredAbility)
	{
		PlayerDerivedAbility = Cast<UGA_PlayerBase>(TargetSlot->Ability->GetDefaultObject());

		if (!PlayerDerivedAbility)
		{
			UE_LOG(VSLog, Warning, TEXT("UAbilityIconWidget::OnModeSwitched: PlayerDerivedAbility is not player derived, can't calculate activation progress."));
		}
	}

	// Prompt an state resync so it matches immediately
	if (TargetSlot->IsAbilityActive())
	{
		SetState(EAbilityState::EAS_Active);
	}
	else
	{
		SetState(EAbilityState::EAS_NotReady);

		// We don't show overheat progress in normal mode
		if (CurrentMode != EDamageType::ENUM_MAX && CurrentMode != EDamageType::EDT_Normal)
		{
			OnOverheatChanged(UGameplayUtilityBlueprintLibrary::GetCorrespondingOverheat(CurrentMode));
		}

		K2_OnStateChanged();
	}
}

void UAbilityIconWidget::OnOverheatChanged(EAttributeType ChangedAttribute)
{
	if (CurrentMode != UGameplayUtilityBlueprintLibrary::GetCorrespondingMode(ChangedAttribute))
	{
		return;
	}

	const float NewValue = BoundGameplaySystem->GetAttributeValue(ChangedAttribute, EAttributeValue::EAV_CurrentValue);

	if (NewValue == LastOverheat)
	{
		return;
	}

	LastOverheat = NewValue;

	if (!TargetSlot)
	{
		UE_LOG(VSLog, Error, TEXT("UAbilityIconWidget::OnOverheatChanged: TargetSlot is null."));
		return;
	}

	if (!PlayerDerivedAbility)
	{
		return;
	}

	const float Progress = LastOverheat / PlayerDerivedAbility->OverheatCost;

	// Don't prompt updates to progress when the ability is ready
	if (AbilityState == EAbilityState::EAS_Active)
	{
		return;
	}

	K2_OnActivationProgressUpdated(Progress);

	if (Progress >= 1.0f)
	{
		SetState(EAbilityState::EAS_Ready);
	}
	else
	{
		SetState(EAbilityState::EAS_NotReady);
	}
}

void UAbilityIconWidget::SetState(EAbilityState InState)
{
	if (AbilityState == InState)
	{
		return;
	}

	AbilityState = InState;

	K2_OnStateChanged();
}
