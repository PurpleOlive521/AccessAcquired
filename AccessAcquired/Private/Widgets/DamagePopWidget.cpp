// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/DamagePopWidget.h"

#include "DamagePopContainerWidget.h"
#include "DevCommons.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UDamagePopWidget::Setup(UDamagePopContainerWidget* InOwningContainer)
{
	check(InOwningContainer);

	OwningContainer = MakeWeakObjectPtr(InOwningContainer);
}

bool UDamagePopWidget::SetAndPlay(const FAAResolvedDamageEvent& InDamageEvent, UCanvasPanel* Canvas)
{
	check(Canvas);

	if (bIsActive)
	{
		UE_LOG(VSLog, Error, TEXT("DamagePopupWidget: Tried to Play already active widget."));
		return false;
	}

	bIsActive = true;
	ParentSlot = Canvas->AddChildToCanvas(this);
	ParentSlot->SetAlignment(DesiredAlignment);

	DamageEvent = InDamageEvent;

	ProjectOverHitLocation();
	K2_SetAndPlay();

	return true;
}

void UDamagePopWidget::ProjectOverHitLocation()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	ensure(PlayerController);

	FVector2D OutLocation = {};
	const bool bInsideScreen = UGameplayStatics::ProjectWorldToScreen(PlayerController, DamageEvent.HitLocation, OutLocation, false);
	const float DPIScaling = UWidgetLayoutLibrary::GetViewportScale(this);
	OutLocation /= DPIScaling;

	if (!bInsideScreen)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	SetVisibility(ESlateVisibility::Visible);
	ParentSlot->SetPosition(OutLocation);
}

bool UDamagePopWidget::IsActive() const
{
	return bIsActive;
}

FVector UDamagePopWidget::GetWorldLocation() const
{
	return DamageEvent.HitLocation;
}

void UDamagePopWidget::FinishPlaying()
{
	if (!bIsActive)
	{
		return;
	}

	bIsActive = false;
	RemoveFromParent();

	if (OwningContainer.IsValid())
	{
		UDamagePopContainerWidget* Container = OwningContainer.Get();
		Container->ReturnWidget(this);
	}
}
