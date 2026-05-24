// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SaveableObjectInterface.h"

#include "DialogueTrigger.generated.h"

UCLASS()
class VERTICALSLICE_API ADialogueTrigger : public AActor, public ISaveableObjectInterface
{
	GENERATED_BODY()
	
public:	
	ADialogueTrigger();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	bool bPreviouslyTriggered = false;
};
