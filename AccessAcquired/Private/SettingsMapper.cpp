// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "SettingsMapper.h"
#include "GameFramework/GameUserSettings.h"
#include "AAHUD.h"
#include "WidgetMenuStack.h"
#include "ConfirmationMenuWidget.h"

bool FSettingsValueContext::operator==(const FSettingsValueContext& Other) const
{
	return
		bState == Other.bState &&
		Value == Other.Value &&
		Magnitude == Other.Magnitude &&
		Vector == Other.Vector &&
		AnonymousEnum == Other.AnonymousEnum;
}

USettingsMapper::USettingsMapper()
{
	auto IsFunctionImplementedInBlueprint = [](const UFunction* Func) -> bool
		{
			return Func && ensure(Func->GetOuter()) && Func->GetOuter()->IsA(UBlueprintGeneratedClass::StaticClass());
		};

	{
		static FName FuncName = FName(TEXT("K2_SyncSettingWithGame"));
		UFunction* BlueprintFunction = GetClass()->FindFunctionByName(FuncName);
		bHasBlueprintSyncSettingWithGame = IsFunctionImplementedInBlueprint(BlueprintFunction);
	}
}

void USettingsMapper::Initialize()
{
	Sync();
}

void USettingsMapper::ApplySetting()
{
	ApplySettingOnGame();

	K2_ApplySettingsOnGame();

	if (UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings())
	{
		GameUserSettings->ApplySettings(true /* bCheckForCommandLineOverrides */);
	}
}

void USettingsMapper::ApplyPendingChange()
{
	if (bHasPendingChange)
	{
		if (RequiresUserConfirmation())
		{
			if (bPromptUserConfirmationAfterApplication)
			{
				ApplySetting();
				PromptUserConfirmation();
				return;
			}
			else
			{
				PromptUserConfirmation();
				return;
			}
		}
		
		ApplyPendingChange_Internal();
	}
}

void USettingsMapper::RemovePendingChange()
{
	const FSettingsValueContext PreRemove = PendingValue;

	bHasPendingChange = false;
	PendingValue = FSettingsValueContext();

	if (PreRemove != FSettingsValueContext())
	{
		ApplySetting();
	}
}

void USettingsMapper::SetValue(const FSettingsValueContext& ValueContext)
{
	if (SettingValue == ValueContext)
	{
		return;
	}

	SettingValue = ValueContext;
	ApplySetting();

	UGameUserSettings::GetGameUserSettings()->RequestUIUpdate();
}

void USettingsMapper::SetPendingValue(const FSettingsValueContext& ValueContext)
{
	if (PendingValue == ValueContext)
	{
		return;
	}

	bHasPendingChange = true;
	PendingValue = ValueContext;
}

bool USettingsMapper::HasPendingChange() const
{
	return bHasPendingChange;
}

void USettingsMapper::Sync()
{
	SyncSetting();
	K2_SyncSetting();
}

FSettingsValueContext USettingsMapper::GetValue() const
{
	if (bHasPendingChange)
	{
		return PendingValue;
	}

	return SettingValue;
}

bool USettingsMapper::GetValueAsBool() const
{
	return GetValue().bState;
}

int32 USettingsMapper::GetValueAsInteger() const
{
	return GetValue().Value;
}

float USettingsMapper::GetValueAsFloat() const
{
	return GetValue().Magnitude;
}

FVector USettingsMapper::GetValueAsVector() const
{
	return GetValue().Vector;
}

uint8 USettingsMapper::GetValueAsEnum() const
{
	return GetValue().AnonymousEnum;
}

bool USettingsMapper::RequiresUserConfirmation() const
{
	bool bResult = false;
	bResult |= bRequiresUserConfirmation;

	bResult |= CheckRequiresUserConfirmation(GetValue());

	if (bHasBlueprintCheckRequiresUserConfirmation)
	{
		bResult |= K2_CheckRequiresUserConfirmation(GetValue());
	}

	return bResult;
}

void USettingsMapper::PromptUserConfirmation()
{
	if (HasPendingChange() && not bWaitingForUserConfirmation)
	{
		if (AAAHUD* Hud = AAAHUD::Get(this))
		{
			if (UWidgetMenuStack* MenuStack = AAAHUD::GetMenuStack(this))
			{
				bWaitingForUserConfirmation = true;

				TSubclassOf<UConfirmationMenuWidget> ConfirmationMenuClass = Hud->GetConfirmationWidgetClass();
				UConfirmationMenuWidget* ConfirmationMenu = MenuStack->PushToModal<UConfirmationMenuWidget>(ConfirmationMenuClass);

				FConfirmationMenuArgs Args;
				Args.Arguments.Add(TEXT(" "));
				ConfirmationMenu->AssignContentAsset(ConfirmationMenuAsset, Args);
				ConfirmationMenu->OnConfirmedDelegate.AddUniqueDynamic(this, &USettingsMapper::ReceiveConfirm);
				ConfirmationMenu->OnCancelledDelegate.AddUniqueDynamic(this, &USettingsMapper::ReceiveCancel);
			}
		}

	}
}

void USettingsMapper::ReceiveConfirm()
{
	bWaitingForUserConfirmation = false;
	ApplyPendingChange_Internal();
}

void USettingsMapper::ReceiveCancel()
{
	bWaitingForUserConfirmation = false;
	RemovePendingChange();

	// We want the backing UI element to update to the reverted value
	UGameUserSettings::GetGameUserSettings()->RequestUIUpdate();
}

void USettingsMapper::ApplyPendingChange_Internal()
{
	SetValue(PendingValue);

	RemovePendingChange();
}

void USettingsMapper::ApplySettingOnGame()
{
}

void USettingsMapper::SyncSetting()
{
}

bool USettingsMapper::CheckRequiresUserConfirmation(const FSettingsValueContext& IncomingValue) const
{
	return false;
}
