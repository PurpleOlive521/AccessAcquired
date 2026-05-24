// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Audio/AAAudioSubsystemSettings.h"

UFMODEvent* UAudioSubsystemSettings::GetRandomBattleMusic() const
{
    if (DefaultBattleMusic.IsEmpty())
    {
        return nullptr;
    }

    int32 RandomIndex = FMath::RandRange(0, DefaultBattleMusic.Num() - 1);

    return DefaultBattleMusic[RandomIndex].LoadSynchronous();
}
