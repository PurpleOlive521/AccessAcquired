// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DialogueWidgetInterface.generated.h"

struct FPackagedDialogueSegment;

UINTERFACE(MinimalAPI)
class UDialogueWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VERTICALSLICE_API IDialogueWidgetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DialogueWidgetInterface")
	void StartDialogue();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DialogueWidgetInterface")
	void QueueDialogue(const FPackagedDialogueSegment& PackagedDialogue);
};
