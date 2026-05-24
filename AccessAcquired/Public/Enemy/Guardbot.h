// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyBase.h"
#include "Guardbot.generated.h"

class UPhaseComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API AGuardbot : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AGuardbot(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPhaseComponent> PhaseComponent = nullptr;
};
