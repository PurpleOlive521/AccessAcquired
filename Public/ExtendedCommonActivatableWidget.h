// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "ExtendedCommonActivatableWidget.generated.h"

class UCommonUIActionRouterBase;
struct FUIInputConfig;

DECLARE_DYNAMIC_DELEGATE_OneParam(FInputActionExecutedDelegate, FName, ActionName);	

USTRUCT(BlueprintType)
struct VERTICALSLICE_API FBP_BindUIActionArgs
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "BindUIActionArgs")
	bool bConsumeInput = false;

	UPROPERTY(BlueprintReadWrite, Category = "BindUIActionArgs")
	bool bDisplayInActionBar = true;

	UPROPERTY(BlueprintReadWrite, Category = "BindUIActionArgs")
	FText OverrideDisplayName;
};

/**
 * Extends the default CommonActivatableWidget behaviour with blueprint-exposed input-binding functions
 */
UCLASS(meta = (DisableNativeTick))
class VERTICALSLICE_API UExtendedCommonActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	// Removes all made bindings
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "ExtendedActivatableWidget")
	void RegisterBinding(FDataTableRowHandle InputAction, const FBP_BindUIActionArgs& InBindArgs, const FInputActionExecutedDelegate& Callback, FUIActionBindingHandle& BindingHandle);

	UFUNCTION(BlueprintCallable, Category = "ExtendedActivatableWidget")
	void UnregisterBinding(FUIActionBindingHandle BindingHandle);

	UFUNCTION(BlueprintCallable, Category = "ExtendedActivatableWidget")
	void UnregisterAllBindings();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExtendedActivatableWidget")
	int GetTotalBindings();

	// Makes sure we don't hold on to expired bindings
	UFUNCTION(BlueprintCallable, Category = "ExtendedActivatableWidget")
	void ValidateBindings();

	UFUNCTION(BlueprintCallable, Category = "ExtendedActivatableWidget")
	void BP_SetActiveUIInputConfig(UCommonUIActionRouterBase* ActionRouterBase, const FUIInputConfig& NewConfig, const UObject* InConfigSource = nullptr);

private:
	TArray<FUIActionBindingHandle> BindingHandles;
};
