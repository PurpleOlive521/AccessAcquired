// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ConfirmationMenuAsset.h"
#include "SettingsMapper.generated.h"

USTRUCT(BlueprintType)
struct FSettingsRotatorOptions
{
	GENERATED_BODY()

	FSettingsRotatorOptions() = default;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FText> TextLabels;
};

// Abstract value wrapper that allows most common settings values to be parsed in.
USTRUCT(BlueprintType)
struct FSettingsValueContext
{
	GENERATED_BODY()

	FSettingsValueContext() = default;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bState = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Value = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Magnitude = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Vector = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 AnonymousEnum = 0U;

	bool operator==(const FSettingsValueContext& Other) const;
};

/**
 * 
 */
UCLASS(Blueprintable)
class VERTICALSLICE_API USettingsMapper : public UObject
{
	GENERATED_BODY()

public:
	USettingsMapper();

	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	void ApplySetting();

	// Applies stored changed value, and prompts user confirmation if needed.
	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	void ApplyPendingChange();

	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	void RemovePendingChange();

	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	void SetValue(const FSettingsValueContext& ValueContext);

	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	void SetPendingValue(const FSettingsValueContext& ValueContext);

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "SettingsMapper")
	bool HasPendingChange() const;

	// Updates Setting value to match the core settings object.
	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	void Sync();

	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	FSettingsValueContext GetValue() const;

	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	bool GetValueAsBool() const;

	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	int32 GetValueAsInteger() const;

	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	float GetValueAsFloat() const;

	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	FVector GetValueAsVector() const;

	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	uint8 GetValueAsEnum() const;

	// Returns true if this setting requires user confirmation when applied.
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "SettingsMapper")
	bool RequiresUserConfirmation() const;

	UFUNCTION(BlueprintCallable, Category = "SettingsMapper")
	void PromptUserConfirmation();

	UFUNCTION()
	void ReceiveConfirm();

	UFUNCTION()
	void ReceiveCancel();

	// The possible values this setting can have when displayed in a Rotator.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SettingsMapper")
	FSettingsRotatorOptions RotatorOptions;

	// Reapplies all settings when value is changed. 
	// Is significantly more expensive for slider-based values that are applied immediately, and not required for custom settings.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SettingsMapper")
	bool bRequiresFullApplySettings = true;

	// A confirmation-popup needs to be accepted by the user before setting is applied on the game.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SettingsMapper")
	bool bRequiresUserConfirmation = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bRequiresUserConfirmation"), Category = "SettingsMapper")
	bool bPromptUserConfirmationAfterApplication = false;

	// The asset to display when setting tries to apply to the game.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bRequiresUserConfirmation"), Category = "SettingsMapper")
	TObjectPtr<UConfirmationMenuAsset> ConfirmationMenuAsset = nullptr;

protected:

	void ApplyPendingChange_Internal();

	virtual void ApplySettingOnGame();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Blueprint Apply Settings on Game"), Category = "SettingsMapper")
	void K2_ApplySettingsOnGame();

	// Updates Setting value to match the core settings object.
	virtual void SyncSetting();

	// Updates Setting value to match the core settings object.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Blueprint Sync Setting"), Category = "SettingsMapper")
	void K2_SyncSetting();

	// Allows for custom requirements for when a user confirmation is requested.
	// Returns true if user confirmation is required when applying setting.
	virtual bool CheckRequiresUserConfirmation(const FSettingsValueContext& IncomingValue) const;

	// Allows for custom requirements for when a user confirmation is requested.
	// Returns true if user confirmation is required when applying setting.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Blueprint Check Requires User Confirmation"), Category = "SettingsMapper")
	bool K2_CheckRequiresUserConfirmation(const FSettingsValueContext& IncomingValue) const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SettingsMapper")
	FSettingsValueContext SettingValue;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SettingsMapper")
	FSettingsValueContext PendingValue;

	bool bHasSyncedWithSettings = false;

	bool bHasPendingChange = false;
	
	bool bWaitingForUserConfirmation = false;

private:

	bool bHasBlueprintSyncSettingWithGame = false;
	bool bHasBlueprintCheckRequiresUserConfirmation = false;
};
