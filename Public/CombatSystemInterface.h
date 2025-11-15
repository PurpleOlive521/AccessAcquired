// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CombatSystemInterface.generated.h"

class UChargeComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatSystemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Allows access to common modules and components expected of combat-participating entities and systems to use.
 * Will still return nullptr when not in use, since not all entities use the same arrangement of systems.
 */
class VERTICALSLICE_API ICombatSystemInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CombatSystemInterface")
	class UChargeComponent* GetPressureComponent();
};
