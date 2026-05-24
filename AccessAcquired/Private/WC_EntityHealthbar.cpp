// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "WC_EntityHealthbar.h"
#include "OverheadStatusWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void UWC_EntityHealthbar::BeginPlay()
{
	Super::BeginPlay();

	SetCastShadow(false);

	UUserWidget* DisplayedWidget = GetWidget();
	if (UOverheadStatusWidget* StatusWidget = Cast<UOverheadStatusWidget>(DisplayedWidget))
	{
		StatusWidget->InitializeWithOwner(GetOwner());
	}
}

void UWC_EntityHealthbar::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsEnabled)
	{
		if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0))
		{
			const float Distance = FVector::Dist(GetComponentLocation(), Player->GetActorLocation());
			const bool bOutsideView = Distance > MaxShownDistance;

			if (bHiddenInGame != bOutsideView)
			{
				SetHiddenInGame(bOutsideView, true);
			}
		}
	}
}


