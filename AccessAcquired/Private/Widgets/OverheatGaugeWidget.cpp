// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/OverheatGaugeWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameFramework/Character.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "PlayerCharacter.h"
#include "OverheatComponent.h"

void UOverheatGaugeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	TargetOverheatAttr = UGameplayUtilityBlueprintLibrary::GetCorrespondingOverheat(TargetMode);
}

void UOverheatGaugeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (IGameplaySystemOwnerInterface* GameplaySystemInterface = Cast<IGameplaySystemOwnerInterface>(PlayerCharacter))
	{
		GameplaySystem = GameplaySystemInterface->Execute_GetGameplaySystemComponent(PlayerCharacter);

		check(GameplaySystem);

		FDelegateCollection& Delegates = GameplaySystem->OnAttributeChangedDelegateCollection;

		Delegates.GetDelegate(EAttributeType::EAT_OverheatLimit).AddUObject(this, &UOverheatGaugeWidget::ListenForOverheat);
		Delegates.GetDelegate(TargetOverheatAttr).AddUObject(this, &UOverheatGaugeWidget::ListenForOverheat);

		Delegates.GetDelegate(EAttributeType::EAT_OverclockThreshold).AddUObject(this, &UOverheatGaugeWidget::ListenForOverclockThreshold);

		// Prompt UI update to immediately match state
		ListenForOverheat(EAttributeType::EAT_NONE);
		ListenForOverclockThreshold(EAttributeType::EAT_NONE);
	}

	if (UModeComponent* ModeComp = PlayerCharacter->GetComponentByClass<UModeComponent>())
	{
		BoundModeComponent = MakeWeakObjectPtr(ModeComp);
		ModeComp->OnModeSwitchedDelegate.AddDynamic(this, &UOverheatGaugeWidget::ListenForModeSwitch);
	}

	if (OverheatComponent = PlayerCharacter->FindComponentByClass<UOverheatComponent>())
	{
		OverheatComponent->OnOverclockedDelegate.AddDynamic(this, &UOverheatGaugeWidget::ListenForOverclock);
	}
}

void UOverheatGaugeWidget::NativeDestruct()
{
	if (GameplaySystem)
	{
		FDelegateCollection& Delegates = GameplaySystem->OnAttributeChangedDelegateCollection;

		Delegates.GetDelegate(EAttributeType::EAT_OverheatLimit).RemoveAll(this);
		Delegates.GetDelegate(TargetOverheatAttr).RemoveAll(this);
	}

	if (UModeComponent* DerefModeComp = BoundModeComponent.Get())
	{
		DerefModeComp->OnModeSwitchedDelegate.RemoveAll(this);
	}

	if (OverheatComponent)
	{
		OverheatComponent->OnOverclockedDelegate.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UOverheatGaugeWidget::ListenForOverheat(EAttributeType OverheatType)
{
	const float OverheatValue = GameplaySystem->GetAttributeValue(TargetOverheatAttr, EAttributeValue::EAV_CurrentValue);
	const float OverheatLimitValue = GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverheatLimit, EAttributeValue::EAV_CurrentValue);

	const float NewPercentage = OverheatValue / OverheatLimitValue;

	K2_OnOverheatChanged(NewPercentage);
}

void UOverheatGaugeWidget::ListenForOverclockThreshold(EAttributeType Attribute)
{
	const float OverclockThreshold = 0.01f * GameplaySystem->GetAttributeValue(EAttributeType::EAT_OverclockThreshold, EAttributeValue::EAV_CurrentValue);

	K2_OnOverclockThresholdChanged(OverclockThreshold);
}

void UOverheatGaugeWidget::ListenForOverclock(bool bIsOverclocked)
{
	K2_OnOverclocked(bIsOverclocked);
}

void UOverheatGaugeWidget::ListenForModeSwitch(EDamageType NewMode)
{
	if (NewMode == TargetMode)
	{
		if (!bIsTargetModeActive)
		{
			bIsTargetModeActive = true;
			K2_OnModeSwitch(true);
		}
	}
	else
	{
		if (bIsTargetModeActive)
		{
			bIsTargetModeActive = false;
			K2_OnModeSwitch(false);
		}
	}
}
