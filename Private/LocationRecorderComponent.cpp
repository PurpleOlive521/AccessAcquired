// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "LocationRecorderComponent.h"

#include "DevCommons.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
ULocationRecorderComponent::ULocationRecorderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULocationRecorderComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();

	if (!CharacterMovement)
	{
		UE_LOG(VSLog, Error, TEXT("LocationRecorderComponent: Could not find CharacterMovementComponent"));
	}
}


// Called every frame
void ULocationRecorderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVector ULocationRecorderComponent::GetRecordedLocation() const
{
	return LastRecordedLocation;
}

