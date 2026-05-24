// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ResponsiveButtonPromptAsset.generated.h"

USTRUCT(BlueprintType)
struct FResponsiveButtonPrompt
{
	GENERATED_BODY()

	// The text will be displayed in a RichText format, and should include the button prompt image.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (MultiLine = true), Category = "ResponsiveButtonPrompt")
	FText Text;

	// The tag that when broadcasted will trigger the button prompt.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ConfirmationMenuContent")
	FGameplayTag TriggeringActionTag;
};

/**
 * Content asset for a number of ResponsiveButtonPromptAsset widget.
 * Determines what button prompt to display, the text used and the GameplayTag-marked action that triggers it.
 */
UCLASS()
class VERTICALSLICE_API UResponsiveButtonPromptAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- Begin UPrimaryDataAsset Interface
	FPrimaryAssetId GetPrimaryAssetId() const override;
	// --- End UPrimaryDataAsset Interface

	// Returns a default constructed FResponsiveButtonPrompt if Index is invalid.
	FResponsiveButtonPrompt GetPromptAtIndex(int32 Index);

	// Displayed in some widgets.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ResponsiveButtonPromptAsset")
	FText TitleText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ResponsiveButtonPromptAsset")
	TArray<FResponsiveButtonPrompt> ButtonPrompts;
};
