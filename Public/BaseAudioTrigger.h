// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseAudioTrigger.generated.h"

UCLASS()
class VERTICALSLICE_API ABaseAudioTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseAudioTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Causes the standard ActorBeginOverlaps logic to be called. Allows for teleporting or streaming objects to activate this object.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AudioTrigger")
	void ForceActorBeginOverlap(AActor* OtherActor);
	virtual void ForceActorBeginOverlap_Implementation(AActor* OtherActor);

	// Causes the standard ActorEndOverlap logic to be called. Allows for teleporting or streaming objects to activate this object.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AudioTrigger")
	void ForceActorEndOverlap(AActor* OtherActor);
	virtual void ForceActorEndOverlap_Implementation(AActor* OtherActor);

};
