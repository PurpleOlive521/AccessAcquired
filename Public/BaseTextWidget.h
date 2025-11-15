// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UBaseTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadWrite)
	FString TargetString = "";

	UPROPERTY(BlueprintReadWrite)
	FString DisplayedString = "";

	UPROPERTY(BlueprintReadWrite)
	int CurrentLetterIndex = 0;

public:
	// Returns true once the entire TargetString has been displayed.
	UFUNCTION(BlueprintCallable)
	bool DisplayNextLetter();

	// Updates any running text effects to adjust for the new text being displayed
	UFUNCTION(BlueprintCallable)
	void UpdateNativeSafely(FText NewText);

	// Resets the typewriter effects internal values. Call before doing subsequent 
	UFUNCTION(BlueprintCallable)
	void ResetInternals();
};
