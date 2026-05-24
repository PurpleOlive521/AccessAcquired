// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Enemy/Guardbot.h"
#include "PhaseComponent.h"

AGuardbot::AGuardbot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PhaseComponent = ObjectInitializer.CreateDefaultSubobject<UPhaseComponent>(this, TEXT("PhaseComponent"));
}
