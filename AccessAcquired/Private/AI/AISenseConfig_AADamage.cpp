// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AISenseConfig_AADamage.h"

UAISenseConfig_AADamage::UAISenseConfig_AADamage(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    DebugColor = FColor::Red;
}

TSubclassOf<UAISense> UAISenseConfig_AADamage::GetSenseImplementation() const
{
    return Implementation;
}
