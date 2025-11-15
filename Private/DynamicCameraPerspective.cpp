// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "DynamicCameraPerspective.h"
#include "DevCommons.h"

// Sets default values
ADynamicCameraPerspective::ADynamicCameraPerspective()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADynamicCameraPerspective::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADynamicCameraPerspective::GetClosestBlendPoint(AActor* TriggerActor)
{
	if (AvailableBlendPoints.Num() == 0)
	{
		UE_LOG(VSLog, Error, TEXT("DynamicCameraPerspective: Error - No available blend points"));
		return;
	}

	if (AvailableBlendPoints.Num() == 1) 
	{
		ClosestPoint = *AvailableBlendPoints.CreateConstIterator();
		return;
	}

	float LowestDistance = 1000000.0f;
	for (USceneComponent* BlendPoint : AvailableBlendPoints) 
	{
		float Distance = FVector::Dist(TriggerActor->GetActorLocation(), BlendPoint->GetComponentLocation());
		if (Distance < LowestDistance)
		{
			LowestDistance = Distance;
			ClosestPoint = BlendPoint;
		}
	}
}

// Called every frame
void ADynamicCameraPerspective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

