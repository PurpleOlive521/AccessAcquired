// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Audio/AAAudioFunctionLibrary.h"
#include "Audio/AAAudioSubsystem.h"

// Can be nullptr, but won't assert and is safe to call anywhere.
#define GET_AUDIO_MANAGER(ParameterName) UAudioSubsystem* ParameterName = UAudioSubsystem::Get(WorldContext);

FFMODEventInstance UAAAudioFunctionLibrary::PlayEvent2D(UObject* WorldContext, UFMODEvent* Event, bool bAutoPlay)
{
    GET_AUDIO_MANAGER(AudioManager);
    if (not AudioManager)
    {
        return FFMODEventInstance();
    }

    return AudioManager->PlayEvent2D(Event, bAutoPlay);
}

FFMODEventInstance UAAAudioFunctionLibrary::PlayEventAtLocation(UObject* WorldContext, UFMODEvent* Event, const FTransform& Location, bool bAutoPlay)
{
    GET_AUDIO_MANAGER(AudioManager);
    if (not AudioManager)
    {
        return FFMODEventInstance();
    }

    return AudioManager->PlayEventAtLocation(Event, Location, bAutoPlay);
}

UFMODAudioComponent* UAAAudioFunctionLibrary::PlayEventAttached(UObject* WorldContext, UFMODEvent* Event, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, bool bAutoPlay, bool bAutoDestroy)
{
    GET_AUDIO_MANAGER(AudioManager);
    if (not AudioManager)
    {
        return nullptr;
    }

    return AudioManager->PlayEventAttached(Event, AttachToComponent, AttachPointName, Location, LocationType, bStopWhenAttachedToDestroyed, bAutoPlay, bAutoDestroy);
}

FFMODEventInstance UAAAudioFunctionLibrary::PlayMusic(UObject* WorldContext, UFMODEvent* Event, bool bIsForeground)
{
    GET_AUDIO_MANAGER(AudioManager);
    if (not AudioManager)
    {
        return FFMODEventInstance();
    }

    return AudioManager->PlayMusic(Event, bIsForeground);
}

FFMODEventInstance UAAAudioFunctionLibrary::ToggleMusicLayer(UObject* WorldContext, bool bPlayForeground)
{
    GET_AUDIO_MANAGER(AudioManager);
    if (not AudioManager)
    {
        return FFMODEventInstance();
    }

    return AudioManager->ToggleMusicLayer(bPlayForeground);
}

FFMODEventInstance UAAAudioFunctionLibrary::PlayFallbackMusic(UObject* WorldContext)
{
    GET_AUDIO_MANAGER(AudioManager);
    if (not AudioManager)
    {
        return FFMODEventInstance();
    }

    return AudioManager->PlayFallbackMusic();
}

void UAAAudioFunctionLibrary::SetFallbackMusic(UObject* WorldContext, UFMODEvent* Event, bool bPlayImmediately)
{
    GET_AUDIO_MANAGER(AudioManager);
    if (not AudioManager)
    {
        return;
    }

    AudioManager->SetFallbackMusic(Event, bPlayImmediately);
}

void UAAAudioFunctionLibrary::StopMusic(UObject* WorldContext, bool bStopForeground, bool bStopBackground)
{
    GET_AUDIO_MANAGER(AudioManager);
    if (not AudioManager)
    {
        return;
    }

    AudioManager->StopMusic(bStopForeground, bStopBackground);
}

bool UAAAudioFunctionLibrary::StopMusicTrack(UObject* WorldContext, UFMODEvent* Event, bool bIsForeground)
{
    GET_AUDIO_MANAGER(AudioManager);
    if (not AudioManager)
    {
        return false;
    }

    return AudioManager->StopMusicTrack(Event, bIsForeground);
}
