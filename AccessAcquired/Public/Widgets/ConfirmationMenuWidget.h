// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/ExtendedCommonActivatableWidget.h"
#include "ConfirmationMenuAsset.h"
#include "ConfirmationMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancelledSignature);

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UConfirmationMenuWidget : public UExtendedCommonActivatableWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "ConfirmationMenu")
	void AssignContentAsset(UConfirmationMenuAsset* Asset, const FConfirmationMenuArgs& Arguments);

	// Blueprints opportunity to display any requested content from the ContentAsset.
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "Refresh Content"), Category = "ConfirmationMenu")
	void K2_RefreshContent(const FConfirmationMenuContent& Content);

	UFUNCTION(BlueprintCallable, Category = "ConfirmationMenu")
	void OnConfirmed();

	UFUNCTION(BlueprintCallable, Category = "ConfirmationMenu")
	void OnCancelled();

	UFUNCTION(BlueprintCallable, Category = "ConfirmationMenu")
	void ClearBinds();

protected:

	UPROPERTY(BlueprintReadOnly, Category = "ConfirmationMenu")
	TObjectPtr<UConfirmationMenuAsset> ContentAsset = nullptr;

	float ElapsedOnScreenTime = 0.0f;

public:
	// --- Delegates;
	UPROPERTY(BlueprintAssignable, Category = "ConfirmationMenu")
	FOnConfirmedSignature OnConfirmedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "ConfirmationMenu")
	FOnCancelledSignature OnCancelledDelegate;
};
