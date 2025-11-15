// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GameFramework/Character.h"

#include "LocationRecorderComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VERTICALSLICE_API ULocationRecorderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULocationRecorderComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> CharacterMovement;

	UPROPERTY(BlueprintReadWrite)
	bool bIsRecordingLocation = false;

	UPROPERTY(BlueprintReadWrite)
	FVector LastRecordedLocation;

	UPROPERTY(BlueprintReadWrite)
	float RecordInterval = 2.0f;

	UPROPERTY(BlueprintReadWrite)
	TSet<AActor*> RecordDisablingActors;

	UPROPERTY(BlueprintReadWrite)
	bool bIsInReturnTransition = false;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Returns true if the record was allowed, false if not.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool TryRecordLocation();

	// Clears any current disablers and tries to log immediately after returned.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ReturnToLastRecordedLocation();

	// Logs callers when disabling recording, so that multiple disabling actors can affect the state at the same time.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ToggleRecording(AActor* Caller, bool ShouldRecord);

	UFUNCTION(BlueprintCallable)
	FVector GetRecordedLocation() const;
		
};
