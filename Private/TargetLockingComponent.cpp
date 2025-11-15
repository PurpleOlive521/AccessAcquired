// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "TargetLockingComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ReceiveDamageEventInterface.h"
#include "DevCommons.h"
#include "MotionWarpingComponent.h"

// Sets default values for this component's properties
UTargetLockingComponent::UTargetLockingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UTargetLockingComponent::BeginPlay()
{
	Super::BeginPlay();

	Camera = GetOwner()->FindComponentByClass<UCameraComponent>();

	if (!Camera)
	{
		UE_LOG(VSLog, Error, TEXT("PlayerCharacter: Error - Could not find CameraComponent"));
	}
	
}

void UTargetLockingComponent::KeepCameraOnTarget(float DeltaTime)
{
	if (bIsLockedOntoTarget == false)
	{
		return;
	}

	FVector CameraWorld = Camera->GetComponentLocation();
	FVector TargetWorld = TargetedActor->GetActorLocation();

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CameraWorld, TargetWorld);

	LookAtRotation.Pitch = FMath::Clamp(LookAtRotation.Pitch, -MinimumAllowedPitch, MinimumAllowedPitch);

	FRotator ControlRotation = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation();

	FRotator FinalRotation = FMath::RInterpTo(ControlRotation, LookAtRotation, DeltaTime, CameraSlewingSpeed);

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(FinalRotation);
}

UMotionWarpingComponent* UTargetLockingComponent::GetMotionWarpingComponent()
{
	if (!MotionWarping)
	{
		MotionWarping = GetOwner()->GetComponentByClass<UMotionWarpingComponent>();
		checkf(MotionWarping, TEXT("No MotionWarpingComponent found on owner Actor!"));
	}

	return MotionWarping;
}


void UTargetLockingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

AActor* UTargetLockingComponent::GetTargetedActor()
{
	return TargetedActor;
}

bool UTargetLockingComponent::TryLockOntoActor(float SearchRadius, FVector SearchOrigin, bool bDebug)
{
	// Procedure:
	// 1. Get all Actors by TargetLocking channel (everything except lockable objects block the hit)
	// 2. Iterate through and add associated Actors to TSet, filtering out non-unique Actors and blocking hits
	// 3. Filter out Actors that are not in the forward direction of the Camera
	// 4. Filter out non-visible  and dead Actors
	// 5. Loop through TSet and get closest Actor based on distance and dot product
	// 6. Assign the closest, unique Actor target to TargetedActor, or nothing if no passing Actor is found
	//

	AActor* Owner = GetOwner();
	bIsLockedOntoTarget = false;

	// --- 1. Collect potential Actors to lock onto ---
	TArray<FHitResult> Hits;

	FCollisionQueryParams SweepQueryParams;
	SweepQueryParams.AddIgnoredActor(Owner);
	SweepQueryParams.bIgnoreBlocks = true;

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SearchRadius);

	GetWorld()->SweepMultiByChannel(Hits, SearchOrigin, SearchOrigin, FQuat::Identity, LockableActorChannel, CollisionShape, SweepQueryParams);

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), Owner->GetActorLocation(), SearchRadius, 12, FColor::Red, false, 5.f);
		PrintToScreen(FString::Printf(TEXT("PlayerCharacter: Found %d Hits"), Hits.Num()));
	}

	if (Hits.Num() == 0)
	{
		return false;
	}



	// --- 2. Filter out non-unique Actors ---
	TSet<AActor*> UniqueActors;

	//Getting all unique Actors hit
	for (const FHitResult& Hit : Hits)
	{
		UniqueActors.Add(Hit.GetActor());
	}

	if (bDebug)
	{
		PrintToScreen(FString::Printf(TEXT("PlayerCharacter: %d Hits left unique Actor filtering"), UniqueActors.Num()));
	}

	if (UniqueActors.Num() == 0)
	{
		return false;
	}



	// --- 3. Copy Hits that are within the Forward direction of the Camera to new TSet ---
	FVector Forward = Camera->GetForwardVector();
	FVector ActorLocation = Owner->GetActorLocation();

	TSet<AActor*> FilteredUniqueActors;

	for (AActor* Hit : UniqueActors)
	{
		FVector HitDirection = Hit->GetActorLocation() - ActorLocation;
		HitDirection.Normalize();

		float DotProduct = FVector::DotProduct(HitDirection, Forward);
		if (DotProduct > ThetaLimit)
		{
			FilteredUniqueActors.Add(Hit);
		}
	}

	if (bDebug)
	{
		PrintToScreen(FString::Printf(TEXT("PlayerCharacter: %d Hits left after Dot product check"), FilteredUniqueActors.Num()));
	}

	if (FilteredUniqueActors.Num() == 0)
	{
		return false;
	}



	// --- 4. Check LOS, copying Actors that are in LOS and Alive to new TSet ---
	FCollisionQueryParams LineTraceQueryParams;
	LineTraceQueryParams.AddIgnoredActor(Owner);

	TSet<AActor*> InLOSFilteredUniqueActors;
	FHitResult Hit;
	FVector Start;
	FRotator _Discard;

	Owner->GetActorEyesViewPoint(Start, _Discard);

	for (AActor* Actor : FilteredUniqueActors)
	{
		GetWorld()->LineTraceSingleByChannel(Hit, Start, Actor->GetActorLocation(), LockableActorChannel, LineTraceQueryParams);

		if (bDebug)
		{
			DrawDebugLine(GetWorld(), Start, Actor->GetActorLocation(), FColor::Yellow, false, 5.f);
		}

		if (!Hit.bBlockingHit)
		{
			// Can be expanded later to call a IsWarpable or IsTargetLockable interface later, to give more flexiblity than only allowed when hit is alive
			bool bIsDead = IReceiveDamageEventInterface::Execute_IsDead(Actor);
			if (bIsDead == true)
			{
				continue;
			}

			InLOSFilteredUniqueActors.Add(Actor);
		}
	}

	if (bDebug)
	{
		PrintToScreen(FString::Printf(TEXT("PlayerCharacter: %d Hits left after LOS and Alive"), InLOSFilteredUniqueActors.Num()));
	}

	if (InLOSFilteredUniqueActors.Num() == 0)
	{
		return false;
	}



	// --- 5. Rate each Actor based on proximity and dot product, and keep track of the one with lowest score ---
	float LowestScore = 100000.f;
	FSetElementId LowestScoreIndex;

	for (const AActor* Actor : InLOSFilteredUniqueActors)
	{
		FVector HitDirection = Actor->GetActorLocation() - ActorLocation;
		HitDirection.Normalize();

		float DotProduct = FVector::DotProduct(HitDirection, Forward);

		// 0 to 1 value, based on how close relative to the maximum possible distance the target is
		float DistanceValue = FVector::Dist(ActorLocation, Actor->GetActorLocation()) / SearchRadius;

		float Score = DistanceValue + 1 - FMath::Abs(DotProduct);

		if (Score < LowestScore)
		{
			LowestScoreIndex = InLOSFilteredUniqueActors.FindId(Actor);
			LowestScore = Score;
		}

		if (bDebug)
		{
			DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), 10, 12, FColor::Red, false, 5.f);
		}
	}



	// --- 6. Return the closest Actor ---
	TargetedActor = InLOSFilteredUniqueActors[LowestScoreIndex];

	if (bDebug)
	{
		PrintToScreen("Targeted Actor:");
		PrintToScreen(TargetedActor->GetName());
	}

	bIsLockedOntoTarget = true;
	return true;
}

bool UTargetLockingComponent::SwitchLockedActor(FVector2D SwitchDirection, float SearchRadius, FVector SearchOrigin, AActor*& PreviousTarget, bool bDebug)
{
	// Procedure:
	// 1. Get all Actors by TargetLocking channel (everything except lockable objects block the hit)
	// 2. Iterate through and add associated Actors to TSet, filtering out non-unique Actors, the already targeted actor and blocking hits
	// 3. Filter out non-visible & dead Actors 
	// 4. Loop through TSet and get the next Actor in the SwitchDirection in screen-space. Evaluated based on distance and dot product from the locked Actor.
	// 5. Assign the closest, unique Actor target, to TargetedActor or nothing if no passing Actor is found
	//

	PreviousTarget = TargetedActor;
	AActor* Owner = GetOwner();

	// Nothing locked, cant switch
	if (!TargetedActor)
	{
		if (bDebug)
		{
			UE_LOG(VSLog, Error, TEXT("PlayerCharacter: Error - No Actor to switch from"));
		}

		return false;
	}

	// --- 1. Collect potential Actors to lock onto ---
	TArray<FHitResult> Hits;

	FCollisionQueryParams SweepQueryParams;
	SweepQueryParams.AddIgnoredActor(Owner);
	SweepQueryParams.bIgnoreBlocks = true;

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SearchRadius);

	GetWorld()->SweepMultiByChannel(Hits, SearchOrigin, SearchOrigin, FQuat::Identity, LockableActorChannel, CollisionShape, SweepQueryParams);

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), Owner->GetActorLocation(), SearchRadius, 12, FColor::Red, false, 5.f);
		PrintToScreen(FString::Printf(TEXT("PlayerCharacter: Found %d Hits"), Hits.Num()));
	}

	if (Hits.Num() == 0)
	{
		return false;
	}

	// --- 2. Filter out non-unique Actors ---
	TSet<AActor*> UniqueActors;

	//Getting all unique Actors hit
	for (const FHitResult& Hit : Hits)
	{
		if (Hit.GetActor() == TargetedActor)
		{
			continue;
		}

		UniqueActors.Add(Hit.GetActor());
	}

	if (bDebug)
	{
		PrintToScreen(FString::Printf(TEXT("PlayerCharacter: %d Hits left unique Actor filtering"), UniqueActors.Num()));
	}

	if (UniqueActors.Num() == 0)
	{
		return false;
	}

	// --- 3. Check LOS, copying Actors that are in LOS & Alive to new TSet ---
	FCollisionQueryParams LineTraceQueryParams;
	LineTraceQueryParams.AddIgnoredActor(Owner);

	TSet<AActor*> InLOSUniqueActors;
	FHitResult Hit;
	FVector Start;
	FRotator _Discard;

	Owner->GetActorEyesViewPoint(Start, _Discard);

	for (AActor* Actor : UniqueActors)
	{
		GetWorld()->LineTraceSingleByChannel(Hit, Start, Actor->GetActorLocation(), LockableActorChannel, LineTraceQueryParams);

		if (bDebug)
		{
			DrawDebugLine(GetWorld(), Start, Actor->GetActorLocation(), FColor::Yellow, false, 5.f);
		}

		if (!Hit.bBlockingHit)
		{
			// Can be expanded later to call a IsWarpable or IsTargetLockable interface later, to give more flexiblity than only allowed when hit is alive
			bool bIsDead = IReceiveDamageEventInterface::Execute_IsDead(Actor);
			if (bIsDead == true)
			{
				continue;
			}

			InLOSUniqueActors.Add(Actor);
		}
	}

	if (bDebug)
	{
		PrintToScreen(FString::Printf(TEXT("PlayerCharacter: %d Hits left after LOS"), InLOSUniqueActors.Num()));
	}

	if (InLOSUniqueActors.Num() == 0)
	{
		return false;
	}


	// --- 4. Get the closest actor based on a value of the product of Distance and Dot Product compared to SwitchDirection ---
	float NearestHitDistance = 100000.f;
	FSetElementId ClosestActorIndex;
	FVector2D Comparator = SwitchDirection.GetSafeNormal();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector2D TargetedActorScreenPos;

	UGameplayStatics::ProjectWorldToScreen(PlayerController, TargetedActor->GetActorLocation(), TargetedActorScreenPos, false);

	for (const AActor* Actor : InLOSUniqueActors)
	{
		FVector2D ScreenPosition;
		bool bSucceeded = UGameplayStatics::ProjectWorldToScreen(PlayerController, Actor->GetActorLocation(), ScreenPosition, false);

		// The Actor is not within the frustum, ignore it
		if (!bSucceeded)
		{
			continue;
		}

		FVector2D LookAtDirection = ScreenPosition - TargetedActorScreenPos;
		LookAtDirection.Normalize();

		float DotProduct = FVector2D::DotProduct(Comparator, LookAtDirection);

		// The Actor is not within the direction of SwitchDirection, ignore it
		if (DotProduct < 0)
		{
			continue;
		}

		float Distance = FVector2D::Distance(TargetedActorScreenPos, ScreenPosition);
		float Product = Distance * DotProduct;

		if (Product < NearestHitDistance)
		{
			ClosestActorIndex = InLOSUniqueActors.FindId(Actor);
			NearestHitDistance = Product;
		}

		if (bDebug)
		{
			DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), 10, 12, FColor::Red, false, 5.f);
		}
	}

	// --- 5. Return the closest Actor ---

	if (ClosestActorIndex.IsValidId())
	{
		TargetedActor = InLOSUniqueActors[ClosestActorIndex];

		if (bDebug)
		{
			PrintToScreen(TargetedActor->GetName());
			PrintToScreen("Switched Targeted Actor:");
		}
	}
	else
	{
		if (bDebug)
		{
			PrintToScreen("No eligible Actor found");
			return false;
		}
	}

	bIsLockedOntoTarget = true;
	return true;
}

void UTargetLockingComponent::OnEnemyDied(AActor* Enemy)
{
	// Is it the current Target that died?
	if (Enemy != TargetedActor) 
	{
		return;
	}

	bIsLockedOntoTarget = false;
	TargetedActor = nullptr;

	// See if we can lock onto another enemy, if one is nearby to keep up the Locking
	RequestTargetLock();
}

void UTargetLockingComponent::WarpToClosestTarget(FVector CameraRelativeInput, bool bDebug)
{
	GetMotionWarpingComponent()->RemoveAllWarpTargets();

	float SearchRadius = WarpSearchDistance;

	// Gather all MotionWarpable objects within a certain radius
	TArray<FHitResult> Hits;

	FVector SearchOrigin = GetOwner()->GetActorLocation();
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SearchRadius);
	FCollisionQueryParams SweepQueryParams;
	SweepQueryParams.AddIgnoredActor(GetOwner());
	SweepQueryParams.bIgnoreBlocks = true;

	GetWorld()->SweepMultiByChannel(Hits, SearchOrigin, SearchOrigin, FQuat::Identity, MotionWarpableChannel, CollisionShape, SweepQueryParams);

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), SearchRadius, 12, FColor::Red, false, 0.5f);
		PrintToScreen(FString::Printf(TEXT("PlayerCharacter: Found %d Warp Targets"), Hits.Num()));
	}

	if (Hits.Num() == 0)
	{
		return;
	}

	// --- Filter out non-unique Actors ---
	TSet<AActor*> UniqueActors;
	TArray<FHitResult> UniqueHits;

	//Getting all unique Actors 
	for (const FHitResult& Hit : Hits)
	{
		bool bIsAlreadyInSet;
		UniqueActors.Add(Hit.GetActor(), &bIsAlreadyInSet);

		if (bIsAlreadyInSet == true)
		{
			continue;
		}

		// Can be expanded later to call a IsWarpable or IsTargetLockable interface later, to give more flexiblity than only allowed when hit is alive
		bool bIsDead = IReceiveDamageEventInterface::Execute_IsDead(Hit.GetActor());
		if (bIsDead == true)
		{
			continue;
		}

		UniqueHits.Emplace(Hit);
	}

	if (bDebug)
	{
		PrintToScreen(FString::Printf(TEXT("PlayerCharacter: %d Warp Targets left after Actor & Alive filtering"), UniqueHits.Num()));
	}

	if (UniqueHits.Num() == 0)
	{
		return;
	}

	FVector SelfForward = GetOwner()->GetActorForwardVector();
	if (CameraRelativeInput.IsNearlyZero() == false)
	{
		CameraRelativeInput.Z = 0;
		SelfForward = CameraRelativeInput;
	}

	// --- Evaluate the Target most in the Forward vector ---
	float HighestDotProduct = 0.0f;
	int BestIndex = 0;
	bool bFirst = true;

	for (int i = 0; i < UniqueHits.Num(); i++)
	{
		FVector HitLocation = UniqueHits[i].GetActor()->GetActorLocation();
		FVector DirectionToHit = HitLocation - GetOwner()->GetActorLocation();
		DirectionToHit.Normalize();
		float DotProduct = FVector::DotProduct(DirectionToHit, SelfForward);

		if (bFirst)
		{
			bFirst = false;
			BestIndex = i;
			HighestDotProduct = DotProduct;
		}

		if (DotProduct > HighestDotProduct)
		{
			BestIndex = i;
			HighestDotProduct = DotProduct;
		}
	}

	AActor* Target = UniqueHits[BestIndex].GetActor();
	if (!Target)
	{
		PrintToScreen("PlayerCharacter: No Actor connected to filter result");
		return;
	}

	FVector TargetLocation = Target->GetActorLocation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation);

	GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName, TargetLocation, TargetRotation);

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), TargetLocation, 30, 12, FColor::Green, false, 5.0f);
		DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), TargetLocation, FColor::Yellow, false, 5.0f);
	}
}

void UTargetLockingComponent::WarpTowardsActor(AActor* Actor, bool bDebug)
{
	if (!Actor)
	{
		PrintToScreen("PlayerCharacter: No Actor to warp towards");
		return;
	}

	FVector TargetLocation = Actor->GetActorLocation();
	FVector SelfLocation = GetOwner()->GetActorLocation();

	// Outside of warp range, get closest point in direction of target
	if (FVector::Distance(SelfLocation, TargetLocation) > WarpSearchDistance)
	{
		FVector Direction = TargetLocation - SelfLocation;
		Direction.Z = SelfLocation.Z; // No Z movement allowed
		Direction.Normalize();

		// Scale up to max distance
		Direction *= WarpSearchDistance;
		TargetLocation = SelfLocation + Direction;
	}

	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation);

	GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName, TargetLocation, TargetRotation);

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), TargetLocation, 30, 12, FColor::Green, false, 5.0f);
		DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), TargetLocation, FColor::Yellow, false, 5.0f);
	}
}

void UTargetLockingComponent::UpdateWarpRotation(FVector CameraRelativeDirection, bool bDebug)
{
	CameraRelativeDirection.Z = 0;

	FVector TargetLocation = GetOwner()->GetActorLocation() + CameraRelativeDirection;
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), TargetLocation);

	GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName, TargetLocation, TargetRotation);

	if (bDebug)
	{
		DrawDebugSphere(GetWorld(), TargetLocation, 10, 12, FColor::Green, false, 1.0f);
		DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + CameraRelativeDirection * 100.0f, FColor::Yellow, false, 1.0f);
	}
}

void UTargetLockingComponent::ClearWarpTargets()
{
	GetMotionWarpingComponent()->RemoveAllWarpTargets();
}


