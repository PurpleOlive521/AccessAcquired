// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DestructibleBase.h"
#include "ReinforcedDestructible.generated.h"



/**
 * 
 */
UCLASS(Blueprintable)
class VERTICALSLICE_API AReinforcedDestructible : public ADestructibleBase
{
	GENERATED_BODY()
public:
	// --- Begin IReceiveDamageEventInterface
	virtual FAAResolvedDamageEvent ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent) override;

	// --- End IReceiveDamageEventInterface


protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDamageType Mode;
	
};
