// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "SaveableObjectInterface.h"
#include "SaveableTriggerVolume.generated.h"

/**
 * A simple TriggerBox that has it's Blueprint SaveGame properties serialized.
 */
UCLASS(Blueprintable)
class VERTICALSLICE_API ASaveableTriggerBox : public ATriggerBox, public ISaveableObjectInterface
{
	GENERATED_BODY()
	
};
