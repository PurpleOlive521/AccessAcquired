// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "ExtendedRichTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UExtendedRichTextBlock : public URichTextBlock
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, Category = "ExtendedRichTextBlock")
	void SetUseOverrideTextStyle(bool InOverrideTextStyle);
};
