// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISenseEvent.h"
#include "AISense_AADamage.h"
#include "AISenseEvent_AADamage.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UAISenseEvent_AADamage : public UAISenseEvent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
	FAISenseAADamageEvent Event;

	virtual FAISenseID GetSenseID() const override;
};
