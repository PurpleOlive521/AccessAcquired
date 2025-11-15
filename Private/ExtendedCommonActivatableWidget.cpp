// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "ExtendedCommonActivatableWidget.h"

#include "CommonUI/Private/Input/UIActionRouterTypes.h"
#include "Input/CommonUIInputTypes.h"
#include "DevCommons.h"

#include "Input/CommonUIActionRouterBase.h"

void UExtendedCommonActivatableWidget::NativeDestruct()
{
	UnregisterAllBindings();

	Super::NativeDestruct();
}

void UExtendedCommonActivatableWidget::RegisterBinding(FDataTableRowHandle InputAction, const FBP_BindUIActionArgs& InBindArgs, const FInputActionExecutedDelegate& Callback, FUIActionBindingHandle& BindingHandle)
{
	// Cleanup first
	ValidateBindings();

	FBindUIActionArgs BindArgs(InputAction, FSimpleDelegate::CreateLambda([InputAction, Callback]()
		{
			Callback.ExecuteIfBound(InputAction.RowName);
		}));

	// --- Override binding arg defaults based on InBindArgs

	BindArgs.bDisplayInActionBar = InBindArgs.bDisplayInActionBar;

	BindArgs.bConsumeInput = InBindArgs.bConsumeInput;

	if (InBindArgs.OverrideDisplayName.IsEmpty() == false)
	{
		BindArgs.OverrideDisplayName = InBindArgs.OverrideDisplayName;
	}

	BindingHandle = RegisterUIActionBinding(BindArgs);
	BindingHandles.Add(BindingHandle);
}

void UExtendedCommonActivatableWidget::UnregisterBinding(FUIActionBindingHandle BindingHandle)
{
	if (BindingHandle.IsValid())
	{
		BindingHandle.Unregister();
		BindingHandles.Remove(BindingHandle);
	}

	ValidateBindings();
}

void UExtendedCommonActivatableWidget::UnregisterAllBindings()
{
	for (FUIActionBindingHandle Handle : BindingHandles)
	{
		if (Handle.IsValid())
		{
			Handle.Unregister();
		}
	}
	BindingHandles.Empty();
}

int UExtendedCommonActivatableWidget::GetTotalBindings()
{
	return BindingHandles.Num();
}

void UExtendedCommonActivatableWidget::ValidateBindings()
{
	TArray<FUIActionBindingHandle> InvalidHandles;

	for (const FUIActionBindingHandle& Handle : BindingHandles)
	{
		if (Handle.IsValid() == false)
		{
			InvalidHandles.Emplace(Handle);
		}
	}

	for (const FUIActionBindingHandle& HandleToRemove : InvalidHandles)
	{
		BindingHandles.Remove(HandleToRemove);
	}
}

void UExtendedCommonActivatableWidget::BP_SetActiveUIInputConfig(UCommonUIActionRouterBase* ActionRouterBase, const FUIInputConfig& NewConfig, const UObject* InConfigSource)
{
	if (IsValid(ActionRouterBase))
	{
		ActionRouterBase->SetActiveUIInputConfig(NewConfig, InConfigSource);
	}
}
