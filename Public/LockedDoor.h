// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SaveableObjectInterface.h"
#include "GuidTag.h"
#include "Components/TimelineComponent.h"

#include "LockedDoor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorStateChangedSignature, bool, bIsOpen);

UCLASS()
class VERTICALSLICE_API ALockedDoor : public AActor, public ISaveableObjectInterface
{
	GENERATED_BODY()
	
public:	
	ALockedDoor();

	// --- Begin AActor Interface
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// --- End AActor Interface

	// Ensures that the ID is not copied over when the Actor is duplicated in the Editor.
	virtual void PostEditImport() override;

	// --- Begin ISaveableObjectInterface
	virtual void SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject) override;

	virtual void LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject) override;
	// --- End ISaveableObjectInterface

	UFUNCTION(BlueprintCallable)
	void OpenDoor();

	UFUNCTION(BlueprintCallable)
	void CloseDoor();

	UFUNCTION(BlueprintCallable)
	void SetDoorState(bool bShouldOpen);

	UFUNCTION(BlueprintCallable)
	void InvertDoorState();

	UFUNCTION(BlueprintCallable)
	void PlayDoorAnimation(bool bShouldOpen);

	// Stops crashes in-editor due to the trying to access the timeline before BeginPlay is called by assigning the timeline component directly.
	UFUNCTION(BlueprintCallable)
	void AssignTimeline(UTimelineComponent* Timeline);

protected:

	// Sets the door to match that of the animation in the given state, without needing to play the animation out first.
	UFUNCTION(BlueprintCallable)
	void MatchDoorAnimationToState(bool bShouldBeOpen);

	UPROPERTY(BlueprintReadWrite)
	bool bIsAnimationPlaying = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsOpen = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bStartOpen = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGuidTag Id;

	TObjectPtr<UTimelineComponent> DoorOpenAnimation;

public:
	// --- Delegates
	UPROPERTY(BlueprintAssignable)
	FOnDoorStateChangedSignature OnDoorStateChangedDelegate;
};
