// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODEvent.h"
#include "FMODBlueprintStatics.h"
#include "AAAudioFunctionLibrary.generated.h"

class UFMODAudioComponent;

// Any call thats part of the library will fail silently if the AudioManager is not available.
UCLASS()
class VERTICALSLICE_API UAAAudioFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContext", DefaultToSelf = "WorldContext", UnsafeDuringActorConstruction = "true"), Category = "Audio|AA")
	static FFMODEventInstance PlayEvent2D(UObject* WorldContext, UFMODEvent* Event, bool bAutoPlay = true);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContext", DefaultToSelf = "WorldContext", UnsafeDuringActorConstruction = "true"), Category = "Audio|AA")
	static FFMODEventInstance PlayEventAtLocation(UObject* WorldContext, UFMODEvent* Event, const FTransform& Location, bool bAutoPlay = true);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContext", DefaultToSelf = "WorldContext", AdvancedDisplay = "3", UnsafeDuringActorConstruction = "true"), Category = "Audio|AA")
	static UFMODAudioComponent* PlayEventAttached(UObject* WorldContext, UFMODEvent* Event, USceneComponent* AttachToComponent, FName AttachPointName,
												  FVector Location, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, bool bAutoPlay = true, bool bAutoDestroy = true);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContext", DefaultToSelf = "WorldContext", UnsafeDuringActorConstruction = "true"), Category = "Audio|AA")
	static FFMODEventInstance PlayMusic(UObject* WorldContext, UFMODEvent* Event, bool bIsForeground = false);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContext", DefaultToSelf = "WorldContext", UnsafeDuringActorConstruction = "true"), Category = "Audio|AA")
	static FFMODEventInstance ToggleMusicLayer(UObject* WorldContext, bool bPlayForeground = false);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContext", DefaultToSelf = "WorldContext", UnsafeDuringActorConstruction = "true"), Category = "Audio|AA")
	static FFMODEventInstance PlayFallbackMusic(UObject* WorldContext);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContext", DefaultToSelf = "WorldContext", UnsafeDuringActorConstruction = "true"), Category = "Audio|AA")
	static void SetFallbackMusic(UObject* WorldContext, UFMODEvent* Event, bool bPlayImmediately);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContext", DefaultToSelf = "WorldContext", UnsafeDuringActorConstruction = "true"), Category = "Audio|AA")
	static void StopMusic(UObject* WorldContext, bool bStopForeground, bool bStopBackground);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContext", DefaultToSelf = "WorldContext", UnsafeDuringActorConstruction = "true"), Category = "Audio|AA")
	static bool StopMusicTrack(UObject* WorldContext, UFMODEvent* Event, bool bIsForeground = false);
};