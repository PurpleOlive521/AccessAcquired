// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#include "Audio/AAAudioSubsystem.h"
#include "GameplayTagMessageDefines.h"
#include "GameplayTagDefines.h"
#include "AAAudioSubsystemSettings.h"

void UAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UGameplayMessageSubsystem::HasInstance(this)) 
    {
        UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

        FGameplayMessageListenerParams<FAAVerbMessage> Params;
        Params.MatchType = EGameplayMessageMatch::ExactMatch;
        Params.SetMessageReceivedCallback(this, &UAudioSubsystem::ReceiveMessage);
        MessageListenerHandle = MessageSubsystem.RegisterListener(GAMEPLAYTAG_Channels_Combat_Player, Params);
    }
}

void UAudioSubsystem::Deinitialize()
{
    if (UGameplayMessageSubsystem::HasInstance(this)) 
    {
        UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

        MessageSubsystem.UnregisterListener(MessageListenerHandle);
    }

    UFMODBlueprintStatics::EventInstanceStop(BackgroundMusicInstance, true);

    UFMODBlueprintStatics::EventInstanceStop(ForegroundMusicInstance, true);

    Super::Deinitialize();
}

void UAudioSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    const UAudioSubsystemSettings* Settings = GetDefault<UAudioSubsystemSettings>();

    if (UFMODEvent* Ambience = Settings->DefaultAmbience.LoadSynchronous()) 
    {
        PlayEvent2D(Ambience);
    }
}

UAudioSubsystem* UAudioSubsystem::Get(const UObject* WorldContext)
{
    if (WorldContext)                                                                  
    {                                                                                   
        if (UWorld* World = WorldContext->GetWorld())                                   
        {                                 
            UAudioSubsystem* AudioSubsystem = WorldContext->GetWorld()->GetSubsystem<UAudioSubsystem>();

            return AudioSubsystem;
        }                                                                               
    }

    return nullptr;
}

UAudioSubsystem* UAudioSubsystem::GetAudioSubsystemForEvent(const UObject* WorldContextObject)
{
    return Get(WorldContextObject);
}

FFMODEventInstance UAudioSubsystem::PlayEvent2D(UFMODEvent* Event, bool bAutoPlay)
{
    return UFMODBlueprintStatics::PlayEvent2D(this, Event, bAutoPlay);
}

FFMODEventInstance UAudioSubsystem::PlayEventAtLocation(UFMODEvent* Event, const FTransform& Location, bool bAutoPlay)
{
    return UFMODBlueprintStatics::PlayEventAtLocation(this, Event, Location, bAutoPlay);
}

UFMODAudioComponent* UAudioSubsystem::PlayEventAttached(UFMODEvent* Event, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, bool bAutoPlay, bool bAutoDestroy)
{
    return UFMODBlueprintStatics::PlayEventAttached(Event, AttachToComponent, AttachPointName, Location, LocationType, bStopWhenAttachedToDestroyed, bAutoPlay, bAutoDestroy);
}

FFMODEventInstance UAudioSubsystem::PlayMusic(UFMODEvent* Event, bool bIsForeground)
{
    if (bIsForeground) 
    {
        // New music requested
        if (Event != ForegroundMusic) 
        {
            StopMusicTrack(ForegroundMusic, bIsForeground);

            ForegroundMusicInstance = PlayEvent2D(Event, true /* bAutoPlay */);
            ForegroundMusic = Event;
            return ToggleMusicLayer(true /* bPlayForeground */);
        }
        else 
        {
            if (not UFMODBlueprintStatics::EventInstanceIsValid(ForegroundMusicInstance))
            {
                ForegroundMusicInstance = PlayEvent2D(Event, true /* bAutoPlay */);
            }
            else
            {
                UFMODBlueprintStatics::EventInstanceSetPaused(ForegroundMusicInstance, false /* bPaused */);
            }

            return ToggleMusicLayer(true /* bPlayForeground */);
        }
    }

    if (Event != BackgroundMusic)
    {
        if (not bIsPlayingForeground)
        {
            UFMODBlueprintStatics::EventInstanceStop(BackgroundMusicInstance, true /* bRelease*/);
            BackgroundMusicInstance = PlayEvent2D(Event, true /* bAutoPlay */);
        }

        BackgroundMusic = Event;
    }
    else
    {
        if (not bIsPlayingForeground)
        {
            UFMODBlueprintStatics::EventInstanceSetPaused(BackgroundMusicInstance, false /* bPaused */);
        }

        return BackgroundMusicInstance;
    }

    return FFMODEventInstance();
}

bool UAudioSubsystem::StopMusicTrack(UFMODEvent* Event, bool bIsForeground)
{
    if (bIsForeground)
    {
        if (ForegroundMusic == Event)
        {
            ForegroundMusic = nullptr;
            UFMODBlueprintStatics::EventInstanceStop(ForegroundMusicInstance, true /* bRelease*/);
            return true;
        }
    }
    else
    {
        if (BackgroundMusic == Event)
        {
            BackgroundMusic = nullptr;
            UFMODBlueprintStatics::EventInstanceStop(BackgroundMusicInstance, true /* bRelease*/);
            return true;
        }
    }

    return false;
}

FFMODEventInstance UAudioSubsystem::ToggleMusicLayer(bool bPlayForeground)
{
    if (bIsPlayingForeground == bPlayForeground) 
    {
        if (bPlayForeground) 
        {
            return ForegroundMusicInstance;
        }
        else 
        {
            return BackgroundMusicInstance;
        }
    }

    bIsPlayingForeground = bPlayForeground;

    if (bIsPlayingForeground) 
    {
        // Music was assigned but never played. Play it now instead of trying to unpause it
        if (not UFMODBlueprintStatics::EventInstanceIsValid(ForegroundMusicInstance))
        {
            ForegroundMusicInstance = PlayEvent2D(ForegroundMusic, true /* bAutoPlay */);
        }
        else
        {
            UFMODBlueprintStatics::EventInstanceSetPaused(ForegroundMusicInstance, false /* bPaused */);
        }

        UFMODBlueprintStatics::EventInstanceSetPaused(BackgroundMusicInstance, true /* bPaused */);

        return ForegroundMusicInstance;
    }
    else
    {
        // Music was assigned but never played. Play it now instead of trying to unpause it
        if (not UFMODBlueprintStatics::EventInstanceIsValid(BackgroundMusicInstance))
        {
            BackgroundMusicInstance = PlayEvent2D(BackgroundMusic, true /* bAutoPlay */);
        }
        else
        {
            UFMODBlueprintStatics::EventInstanceSetPaused(BackgroundMusicInstance, false /* bPaused */);
        }

        UFMODBlueprintStatics::EventInstanceStop(ForegroundMusicInstance, true /* bRelease */);
    }

    return FFMODEventInstance();
}

FFMODEventInstance UAudioSubsystem::PlayFallbackMusic()
{
    return PlayMusic(FallbackMusic);
}

void UAudioSubsystem::SetFallbackMusic(UFMODEvent* Event, bool bPlayImmediately)
{
    FallbackMusic = Event;

    if (bPlayImmediately) 
    {
        PlayMusic(FallbackMusic);
    }
}

void UAudioSubsystem::StopMusic(bool bStopForeground, bool bStopBackground)
{
    if (bStopForeground) 
    {
        UFMODBlueprintStatics::EventInstanceStop(ForegroundMusicInstance, true /* Release */);
        ForegroundMusic = nullptr;
    }

    if (bStopBackground)
    {
        UFMODBlueprintStatics::EventInstanceStop(BackgroundMusicInstance, true /* Release */);
        ForegroundMusic = nullptr;
    }
}

void UAudioSubsystem::ReceiveMessage(FGameplayTag Channel, const FAAVerbMessage& Message)
{
    const UAudioSubsystemSettings* Settings = GetDefault<UAudioSubsystemSettings>();

    if (Message.Verb == GAMEPLAYTAG_Verbs_EnteredCombat) 
    {
        UFMODEvent* MusicToPlay = nullptr;

        if (Message.TargetTags.HasTag(GAMEPLAYTAG_Enemy_Status_StrongEnemy)) 
        {
            MusicToPlay = Settings->StrongEnemyBattleMusic.LoadSynchronous();
        }
        else 
        {
            MusicToPlay = Settings->GetRandomBattleMusic();
        }

        PlayMusic(MusicToPlay, true /* bAutoPlay */);

        return;
    }

    if (Message.Verb == GAMEPLAYTAG_Verbs_ExitedCombat)
    {
        ToggleMusicLayer(false /* bPlayForeground*/);
        return;
    }
}