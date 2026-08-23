// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayMessages/GameplayMessageTypes.h"

#include "AAAudioSubsystem.generated.h"

class UFMODAudioComponent;

// The Audio Subsystem acts as a wrapper around Unreal and FMOD, and is tailored to Access Acquired.
UCLASS()
class VERTICALSLICE_API UAudioSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:	

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// Helper getter
	static UAudioSubsystem* Get(const UObject* WorldContext);

	// Same as UAudioSubsystem::Get but for Blueprint with exposed WorldContextObject.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayEvent|Helpers")
	static UAudioSubsystem* GetAudioSubsystemForEvent(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "AudioSubsystem")
	FFMODEventInstance PlayEvent2D(UFMODEvent* Event, bool bAutoPlay = true);

	UFUNCTION(BlueprintCallable, Category = "AudioSubsystem")
	FFMODEventInstance PlayEventAtLocation(UFMODEvent* Event, const FTransform& Location, bool bAutoPlay = true);

	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = "2"), Category = "AudioSubsystem")
	UFMODAudioComponent* PlayEventAttached(UFMODEvent* Event, USceneComponent* AttachToComponent, FName AttachPointName,
		FVector Location, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, bool bAutoPlay = true, bool bAutoDestroy = true);

	UFUNCTION(BlueprintCallable, Category = "AudioSubsystem")
	FFMODEventInstance PlayMusic(UFMODEvent* Event, bool bIsForeground = false);

	UFUNCTION(BlueprintCallable, Category = "AudioSubsystem")
	bool StopMusicTrack(UFMODEvent* Event, bool bIsForeground = false);

	UFUNCTION(BlueprintCallable, Category = "AudioSubsystem")
	FFMODEventInstance ToggleMusicLayer(bool bPlayForeground = false);

	UFUNCTION(BlueprintCallable, Category = "AudioSubsystem")
	FFMODEventInstance PlayFallbackMusic();

	UFUNCTION(BlueprintCallable, Category = "AudioSubsystem")
	void SetFallbackMusic(UFMODEvent* Event, bool bPlayImmediately);

	UFUNCTION(BlueprintCallable, Category = "AudioSubsystem")
	void StopMusic(bool bStopForeground, bool bStopBackground);

protected:

	void ReceiveMessage(FGameplayTag Channel, const FAAVerbMessage& Message);
	
	UPROPERTY()
	TObjectPtr<UFMODEvent> FallbackMusic = nullptr;

	FFMODEventInstance BackgroundMusicInstance;

	UFMODEvent* BackgroundMusic;

	FFMODEventInstance ForegroundMusicInstance;

	UFMODEvent* ForegroundMusic;

	bool bIsPlayingForeground = false;

private:

	FGameplayMessageListenerHandle MessageListenerHandle;
};