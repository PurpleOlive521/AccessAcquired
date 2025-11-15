// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "FMODEvent.h"

#include "AudioManager.generated.h"

class UFMODAudioComponent;

UCLASS()
class VERTICALSLICE_API AAudioManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAudioManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (AdvancedDisplay = "2"), Category = "AudioManager")
	FFMODEventInstance PlayEvent2D(UFMODEvent* Event, bool bAutoPlay = true);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (AdvancedDisplay = "2"), Category = "AudioManager")
	FFMODEventInstance PlayEventAtLocation(UFMODEvent* Event, const FTransform& Location, bool bAutoPlay = true);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (AdvancedDisplay = "2"), Category = "AudioManager")
	class UFMODAudioComponent* PlayEventAttached(UFMODEvent* Event, USceneComponent* AttachToComponent, FName AttachPointName,
		FVector Location, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, bool bAutoPlay = true, bool bAutoDestroy = true);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (AdvancedDisplay = "2"), Category = "AudioManager")
	FFMODEventInstance PlayMusic(UFMODEvent* Event, bool bIsForeground = false);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (AdvancedDisplay = "2"), Category = "AudioManager")
	FFMODEventInstance ToggleMusicLayer(bool bPlayForeground = false);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (AdvancedDisplay = "2"), Category = "AudioManager")
	FFMODEventInstance PlayFallbackMusic();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (AdvancedDisplay = "2"), Category = "AudioManager")
	void SetFallbackMusic(UFMODEvent* Event, bool bPlayImmediately);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (AdvancedDisplay = "2"), Category = "AudioManager")
	void StopMusic(bool bStopForeground, bool bStopBackground);

};
