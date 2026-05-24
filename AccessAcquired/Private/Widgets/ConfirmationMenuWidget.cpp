// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/ConfirmationMenuWidget.h"

#include "ConfirmationMenuAsset.h"

void UConfirmationMenuWidget::NativeOnActivated() 
{
	Super::NativeOnActivated();

	ElapsedOnScreenTime = 0.0f;
}

void UConfirmationMenuWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

void UConfirmationMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ElapsedOnScreenTime += InDeltaTime;

	if (IsActivated())
	{
		if (ContentAsset && ContentAsset->HasMaxDisplayTime())
		{
			if (ElapsedOnScreenTime >= ContentAsset->MaxDisplayTime)
			{
				OnCancelled();
				return;
			}

			// TODO: Look into a way to represent this need for updating the text args without needing to recreate the entire widget state each time
			const float TimeRemaining = fabs(ElapsedOnScreenTime - ContentAsset->MaxDisplayTime);
			const FString TimeRemainingAsString = FString::Printf(TEXT("%.1f s"), TimeRemaining);

			FConfirmationMenuArgs Args;
			Args.Arguments.Add(TimeRemainingAsString);
			AssignContentAsset(ContentAsset, Args);
		}
	}
}

void UConfirmationMenuWidget::AssignContentAsset(UConfirmationMenuAsset* Asset, const FConfirmationMenuArgs& Arguments)
{
	ensure(Asset);

	if (Asset)
	{
		ContentAsset = Asset;

		const FConfirmationMenuContent& Content = ContentAsset->GetContent(Arguments);

		K2_RefreshContent(Content);
	}
}

void UConfirmationMenuWidget::OnConfirmed()
{
	OnConfirmedDelegate.Broadcast();

	ClearBinds();

	DeactivateWidget();
}

void UConfirmationMenuWidget::OnCancelled() 
{
	OnCancelledDelegate.Broadcast();

	ClearBinds();

	DeactivateWidget();
}

void UConfirmationMenuWidget::ClearBinds() 
{
	OnConfirmedDelegate.Clear();
	OnCancelledDelegate.Clear();

	UnregisterAllBindings();
}
