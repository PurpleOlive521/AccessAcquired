// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "BaseAudioTrigger.h"

// Sets default values
ABaseAudioTrigger::ABaseAudioTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseAudioTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseAudioTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseAudioTrigger::ForceActorBeginOverlap_Implementation(AActor* OtherActor)
{
}

void ABaseAudioTrigger::ForceActorEndOverlap_Implementation(AActor* OtherActor)
{
}

