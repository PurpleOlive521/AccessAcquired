// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/OverheatWarningsWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

void UOverheatWarningsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	UOverheatComponent* Component = PlayerCharacter->FindComponentByClass<UOverheatComponent>();
	OverheatComponent = MakeWeakObjectPtr(Component);
	if (Component)
	{
		OverheatComponent->OnChangeSeverityDelegate.AddDynamic(this, &UOverheatWarningsWidget::OnSeverityChanged);
		OverheatComponent->OnModeOverheatedDelegate.AddDynamic(this, &UOverheatWarningsWidget::OnModeOverheated);
	}
}

void UOverheatWarningsWidget::NativeDestruct()
{
	if (UOverheatComponent* DerefComponent = OverheatComponent.Get())
	{
		DerefComponent->OnChangeSeverityDelegate.RemoveAll(this);
		DerefComponent->OnModeOverheatedDelegate.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UOverheatWarningsWidget::OnSeverityChanged(EOverheatSeverity Severity)
{
	if (Severity >= ActivationSeverity)
	{
		if (bIsWarningActive)
		{
			return;
		}

		K2_ActivateWarning();
		bIsWarningActive = true;
	}
	else
	{
		if (!bIsWarningActive)
		{
			return;
		}

		K2_DeactivateWarning();
		bIsWarningActive = false;
	}

}

void UOverheatWarningsWidget::OnModeOverheated(bool bEnteredOverheat)
{
	K2_OnOverheated(bEnteredOverheat);
}
