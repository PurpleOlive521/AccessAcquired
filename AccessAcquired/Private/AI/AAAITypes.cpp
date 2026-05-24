// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AAAITypes.h"

FKnownTargetData::FKnownTargetData(AActor* InTarget)
{
    check(InTarget);
    Target = MakeWeakObjectPtr(InTarget);
}

AActor* FKnownTargetData::GetTarget() const
{
    return Target.Get();
}

uint32 GetTypeHash(const FKnownTargetData& InHandle)
{
    return PointerHash(InHandle.Target.Get());
}
