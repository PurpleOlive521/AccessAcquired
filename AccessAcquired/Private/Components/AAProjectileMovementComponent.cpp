// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Components/AAProjectileMovementComponent.h"

void UAAProjectileMovementComponent::SetInstigator(AActor* InInstigator)
{
	check(InInstigator);

	Instigator = MakeWeakObjectPtr(InInstigator);
}

void UAAProjectileMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	if (bIgnoreCollisionsWithInstigator)
	{
		if (Hit.GetActor() == Instigator.Get())
		{
		}
	}

	Super::HandleImpact(Hit, TimeSlice, MoveDelta);
}
