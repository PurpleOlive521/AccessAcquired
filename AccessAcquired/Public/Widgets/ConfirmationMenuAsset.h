// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ConfirmationMenuAsset.generated.h"

USTRUCT(BlueprintType)
struct FConfirmationMenuArgs
{
	GENERATED_BODY()

	// Returns an empty string if we don't have requested id.
	FString GetStringArgumentAt(int32 Index) const;

	// These will be inserted into any text that matches the ReplacementToken.
	// The index of the elements needs to match the order they appear in FConfirmationMenuContent.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ConfirmationMenuArgs")
	TArray<FString> Arguments;
};

USTRUCT(BlueprintType)
struct FConfirmationMenuContent 
{
	GENERATED_BODY()

	// Replaces any text found that matches the ReplacementToken.
	void ReplaceTokens(const FConfirmationMenuArgs& Arguments);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ConfirmationMenuContent")
	FText TitleText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (MultiLine = true), Category = "ConfirmationMenuContent")
	FText BodyText;

	// The left button in the widget. Always displayed in uppercase.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ConfirmationMenuContent")
	FText ConfirmText;

	// The right button in the widget. Always displayed in uppercase.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ConfirmationMenuContent")
	FText CancelText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ConfirmationMenuContent")
	FString ReplacementToken = TEXT("{%}");
};

struct FConfirmationMenuConstants
{
	static const float NO_MAX_DISPLAY_TIME;
};

/**
 * Content asset for a ConfirmationMenu widget.
 * Determines what content to display for the title, description and options.
 */
UCLASS()
class VERTICALSLICE_API UConfirmationMenuAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- Begin UPrimaryDataAsset Interface
	FPrimaryAssetId GetPrimaryAssetId() const override;
	// --- End UPrimaryDataAsset Interface

	UFUNCTION(BlueprintCallable, Category = "ConfirmationMenuAsset")
	FConfirmationMenuContent GetContent(const FConfirmationMenuArgs& Arguments) const;

	UFUNCTION(BlueprintCallable, Category = "ConfirmationMenuAsset")
	bool HasMaxDisplayTime() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ConfirmationMenuAsset")
	FConfirmationMenuContent Content;

	// Once MaxDisplayTime is reached, the ConfirmationWidget is closed automatically with the Cancel option chosen.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ConfirmationMenuAsset")
	float MaxDisplayTime = FConfirmationMenuConstants::NO_MAX_DISPLAY_TIME;
};
