// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "TargetLockingComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "ReceiveDamageEventInterface.h"
#include "DevelopmentTypes.h"
#include "DevCommons.h"
#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "GameplayTagMessageDefines.h"
#include "TargetLockableComponent.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

constexpr bool DEBUG_ENABLED = false;
constexpr int32 INVALID_SWITCH_INDEX = -1;
constexpr float SNAP_DISTANCE_THRESHOLD = 1.0f;

UTargetLockingComponent::UTargetLockingComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTargetLockingComponent::BeginPlay()
{
	Super::BeginPlay();

	Camera = GetOwner()->FindComponentByClass<UCameraComponent>();
	if (!Camera)
	{
		// No camera to act on, disable it
		bEnableTargetLocking = false;
	}

	GameplaySystem = GetOwner()->GetComponentByClass<UGameplaySystemComponent>();
	if (!GameplaySystem)
	{
		UE_LOG(VSLog, Error, TEXT("TargetLockingComponent: Could not find GameplaySystemComponent"));
	}

	if (bEnableTargetLocking)
	{
		InitializeTargetLocking();
	}
}

void UTargetLockingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bEnableTargetLocking)
	{
		if (bIsLockedOntoTarget)
		{
			CurrentDelta = FMath::Clamp(CurrentDelta + DeltaTime * TargetLockingParams.IconSlewSpeed, 0.0f, 1.0f);

			const FVector CurrentLocation = WidgetComponent->GetComponentLocation();
			const FVector TargetLocation = LockedComponent->GetDesiredIconLocation();

			const FVector NewLocation = FMath::Lerp(CurrentLocation, TargetLocation, CurrentDelta);
			WidgetComponent->SetWorldLocation(NewLocation);
		}
	}
}

void UTargetLockingComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (UWorld* World = GetWorld())
	{
		if (UGameplayMessageSubsystem::HasInstance(this))
		{
			UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(this);

			MessageSystem->UnregisterListener(VerbHandle);
			MessageSystem->UnregisterListener(RewardHandle);
		}
	}
}

TArray<FName> UTargetLockingComponent::GetCollisionProfiles()
{
	TArray<TSharedPtr<FName>> List;
	UCollisionProfile::GetProfileNames(List);

	TArray<FName> OutList;

	for (auto SharedName : List)
	{
		if (FName* DerefName = SharedName.Get())
		{
			OutList.Emplace(*DerefName);
		}
	}

	return OutList;
}

void UTargetLockingComponent::TryLock()
{
	if (not bEnableTargetLocking)
	{
		return;
	}

	StopLocking();

	TArray<UTargetLockableComponent*> LockableComps;
	FindLockableComponents(LockableComps);

	if (LockableComps.IsEmpty())
	{
		return;
	}

	if (LockableComps.Num() == 1)
	{
		LockOnTarget(LockableComps[0]);
		return;
	}

	const FVector Forward = Camera->GetForwardVector();
	const FVector SelfLocation = GetOwner()->GetActorLocation();

	float LowestScore = FLT_MAX;
	int32 LowestScoreIndex = 0;

	int32 Index = 0;
	for (UTargetLockableComponent* LockableComp : LockableComps)
	{
		AActor* LockableOwner = LockableComp->GetOwner();
		FVector HitDirection = LockableOwner->GetActorLocation() - SelfLocation;
		HitDirection.Normalize();

		const float DotProduct = FVector::DotProduct(HitDirection, Forward);

		// 0 to 1 value, based on how close relative to the maximum possible distance the target is
		const float DistanceValue = FVector::Dist(SelfLocation, LockableOwner->GetActorLocation()) / TargetLockingParams.SearchRadius;

		const float Score = DistanceValue + 1 - FMath::Abs(DotProduct);
		if (Score < LowestScore)
		{
			LowestScoreIndex = Index;
			LowestScore = Score;
		}

		if (DEBUG_ENABLED)
		{
			DrawDebugSphere(GetWorld(), LockableOwner->GetActorLocation(), 10, 12, FColor::Red, false, 5.f);
		}

		Index++;
	}

	LockOnTarget(LockableComps[LowestScoreIndex]);
	return;
}

void UTargetLockingComponent::SwitchTarget(FVector2D SwitchDirection)
{
	if (SwitchDirection.Length() < TargetLockingParams.SwitchTargetDeadzone)
	{
		return;
	}

	if (not bIsLockedOntoTarget)
	{
		return;
	}

	if (not CanSwitchTarget())
	{
		return;
	}

	TArray<UTargetLockableComponent*> LockableComps;
	FindLockableComponents(LockableComps, LockedComponent);

	AActor* Owner = GetOwner();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	float NearestHitDistance = FLT_MAX;
	int32 ClosestActorIndex = INVALID_SWITCH_INDEX;

	const FVector2D Comparator = SwitchDirection.GetSafeNormal();
	FVector2D TargetedActorScreenPos;

	UGameplayStatics::ProjectWorldToScreen(PlayerController, GetTargetedActor()->GetActorLocation(), TargetedActorScreenPos, false);

	int32 Index = -1;
	for (UTargetLockableComponent* LockableComp : LockableComps)
	{
		Index++;

		AActor* LockableOwner = LockableComp->GetOwner();

		FVector2D ScreenPosition;
		bool bInFrustum = UGameplayStatics::ProjectWorldToScreen(PlayerController, LockableOwner->GetActorLocation(), ScreenPosition, false);
		if (!bInFrustum)
		{
			continue;
		}

		FVector2D LookAtDirection = ScreenPosition - TargetedActorScreenPos;
		LookAtDirection.Normalize();

		float DotProduct = FVector2D::DotProduct(Comparator, LookAtDirection);
		if (DotProduct < 0)
		{
			// The Actor is not within the direction of SwitchDirection, ignore it
			continue;
		}

		float Distance = FVector2D::Distance(TargetedActorScreenPos, ScreenPosition);
		float Product = Distance * DotProduct;

		if (Product < NearestHitDistance)
		{
			ClosestActorIndex = Index;
			NearestHitDistance = Product;
		}

		if (DEBUG_ENABLED)
		{
			DrawDebugSphere(GetWorld(), LockableOwner->GetActorLocation(), 10, 12, FColor::Red, false, 5.f);
		}
	}

	if (ClosestActorIndex == INVALID_SWITCH_INDEX)
	{
		return;
	}

	LockOnTarget(LockableComps[ClosestActorIndex]);

	FGameplayEffectHandle Handle;
	GameplaySystem->AddGameplayEffectFromType(TargetLockingParams.DisableSwitchTargetEffect, Handle, GetOwner());
}

AActor* UTargetLockingComponent::GetTargetedActor() const
{
	if (LockedComponent)
	{
		return LockedComponent->GetOwner();
	}

	return nullptr;
}

bool UTargetLockingComponent::IsLockedOntoTarget() const
{
	return IsValid(LockedComponent);
}

void UTargetLockingComponent::LockOnTarget(UTargetLockableComponent* NewTarget)
{
	if (not bEnableTargetLocking)
	{
		return;
	}

	// Already locked to it
	if (LockedComponent == NewTarget)
	{
		return;
	}

	if (bIsLockedOntoTarget)
	{
		LockedComponent = NewTarget;
		CurrentDelta = 0.0f;
		K2_SwitchTarget();
		return;
	}

	if (!LockedComponent)
	{
		WidgetComponent->SetWorldLocation(NewTarget->GetDesiredIconLocation());
	}

	if (DEBUG_ENABLED)
	{
		PrintToScreen("TargetLockingComponent: Targeted Actor:");
		PrintToScreen(LockedComponent->GetOwner()->GetName());
	}

	LockedComponent = NewTarget;

	bIsLockedOntoTarget = true;

	WidgetComponent->SetVisibility(true /* bNewVisibility */, true /* bPropagateToChildren */);

	CurrentDelta = 0.0f;

	K2_StartLockingOnTarget();
}

void UTargetLockingComponent::StopLocking()
{
	if (!bIsLockedOntoTarget)
	{
		return;
	}

	bIsLockedOntoTarget = false;

	LockedComponent = nullptr;

	WidgetComponent->SetVisibility(false /* bNewVisibility */, true /* bPropagateToChildren */);

	k2_StopLocking();
}

void UTargetLockingComponent::TriggerMotionWarp(const FMotionWarpQuery& QueryParams)
{
	// No distance is set, don't create a MotionWarpingTarget
	const float WarpDistance = CalculateWarpDistance(QueryParams);
	if (WarpDistance <= 0.0f)
	{
		return;
	}

	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	FVector Target = {};
	FRotator Rotation = {};

	// We have override direction, use it
	if (OverrideDirection != FVector::ZeroVector)
	{
		Target = OwnerLocation + OverrideDirection * WarpDistance;
		Rotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, Target);

		// Try to find a target that is close to the direction
		if (AActor* FoundTarget = FindWarpableTarget(DiscoverTargetRadius))
		{
			const FVector Direction = (FoundTarget->GetActorLocation() - OwnerLocation).GetSafeNormal();
			const FVector AssistTarget = OwnerLocation + Direction * WarpDistance;
			const FRotator AssistRotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, AssistTarget);

			const float DirectionDifference = fabs((AssistRotation - Rotation).Yaw);

			// Use the target if its close enough to the input direction, like we do for EWarpSource::EWS_Discovered 
			if (DirectionDifference <= MaximumAssistedInputDifference)
			{
				Target = AssistTarget;
				Rotation = AssistRotation;
			}
		}

		WarpSource = EWarpSource::EWS_OverrideInput;
	}
	// We have a targeted Actor, use it
	else if(AActor* TargetActor = GetTargetedActor())
	{
		const FVector Direction = (TargetActor->GetActorLocation() - OwnerLocation).GetSafeNormal();
		Target = OwnerLocation + Direction * WarpDistance;
		Rotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, Target);

		WarpSource = EWarpSource::EWS_TargetLocking;
	}
	// Try to find an target to use
	else if(AActor* FoundTarget = FindWarpableTarget(DiscoverTargetRadius))
	{
		const FVector Direction = (FoundTarget->GetActorLocation() - OwnerLocation).GetSafeNormal();
		Target = OwnerLocation + Direction * WarpDistance;
		Rotation = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, Target);

		WarpSource = EWarpSource::EWS_Discovered;
	}
	else 
	{
		// No target found, don't apply MotionWarping
		WarpSource = EWarpSource::EWS_None;
		return;
	}

	// Note: We discard Z since we don't apply it in the MotionWarping anyways
	const float Distance = (FVector2D(Target) - FVector2D(OwnerLocation)).Length();
	if (Distance < WarpTargetRadius)
	{
		Target = OwnerLocation;
	}

	// If there is a blocking body between us and the target, use the blocking hit as our furthest point to travel.
	FVector OutLocation = {};
	const bool bIsWarpTargetBlocked = IsWarpTargetBlocked(Target, OutLocation);
	if (bIsWarpTargetBlocked)
	{
		Target = OutLocation;
	}

	AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName, Target, Rotation);

	if (DEBUG_ENABLED)
	{
		DrawDebugSphere(GetWorld(), Target, 30, 12, FColor::Green, false, 5.0f);
		DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), Target, FColor::Yellow, false, 5.0f);
	}
}

AActor* UTargetLockingComponent::FindWarpableTarget(float SearchDistance)
{
	TArray<AActor*> FoundTargets;

	FindWarpableActors(SearchDistance, FoundTargets);

	// No candidates found
	if (FoundTargets.Num() == 0)
	{
		return nullptr;
	}

	FVector SelfForward = GetOwner()->GetActorForwardVector();

	// Evaluate the Target most in the Forward vector
	float HighestDotProduct = FLT_MIN;
	int BestIndex = 0;

	for (int i = 0; i < FoundTargets.Num(); i++)
	{
		FVector HitLocation = FoundTargets[i]->GetActorLocation();
		FVector DirectionToHit = HitLocation - GetOwner()->GetActorLocation();
		DirectionToHit.Normalize();
		float DotProduct = FVector::DotProduct(DirectionToHit, SelfForward);

		if (DotProduct > HighestDotProduct)
		{
			BestIndex = i;
			HighestDotProduct = DotProduct;
		}
	}

	return FoundTargets[BestIndex];
}

void UTargetLockingComponent::SetOverrideDirection(FVector2D Direction)
{
	const FVector ConvertedInput = FVector{ Direction, 0.0f };

	if (OverrideDirection != ConvertedInput)
	{
		OverrideDirection = ConvertedInput;
		RotateWarpTarget(Direction);
	}
}

void UTargetLockingComponent::RotateWarpTarget(FVector2D Direction)
{
	if (const FMotionWarpingTarget* CurrentWarpTarget = FindWarpTarget(WarpTargetName))
	{	
		const FVector Origin = GetOwner()->GetActorLocation();
		const FVector Offset = CurrentWarpTarget->GetLocation() - Origin;

		FVector2D CurrentDir = FVector2d(Offset.X, Offset.Y).GetSafeNormal();
		if (CurrentDir.IsNearlyZero())
		{
			return;
		}

		const float Cross = FVector2D::CrossProduct(CurrentDir, Direction);
		const float Dot = FVector2D::DotProduct(CurrentDir, Direction);
		const float AngleRad = FMath::Atan2(Cross, Dot);

		const FQuat RotationQuat = FQuat(FVector::UpVector, AngleRad);
		const FVector RotatedOffset = RotationQuat.RotateVector(Offset);

		const FVector NewTarget = Origin + RotatedOffset;
		const FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(Origin, NewTarget);

		AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName, NewTarget, NewRotation);
	}
}

float UTargetLockingComponent::CalculateWarpDistance(const FMotionWarpQuery& QueryParams) const
{
	if (GameplaySystem)
	{
		if (QueryParams.bScaleBySpeed)
		{
			const float MovementSpeed = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MovementSpeed, EAttributeValue::EAV_CurrentValue);
			const float Velocity = GameplaySystem->GetOwner()->GetVelocity().Length();

			// Moving faster by physics manipulation, impulse, falling, etc. should not affect the value.
			const float RelativeSpeed = FMath::Clamp(Velocity / MovementSpeed, 0.0f, 1.0f);

			return QueryParams.Distance + RelativeSpeed * MovementSpeed * QueryParams.SpeedScalar;
		}
	}

	return QueryParams.Distance;
}

void UTargetLockingComponent::RemoveComponentWarpTarget()
{
	RemoveWarpTarget(WarpTargetName);
}

bool UTargetLockingComponent::CanSwitchTarget() const
{
	return GameplaySystem->HasTag(GAMEPLAYTAG_Player_TargetLockSwitchDisabled) == false;
}

void UTargetLockingComponent::FindLockableComponents(TArray<UTargetLockableComponent*>& OutComponents, UTargetLockableComponent* Ignore)
{
	// We filter out Hits that are:
	//		- Non-unique or blocking
	//		- Not in the forward direction of the Camera
	//		- Not in LOS
	//		- Implement IReceiveDamageEventInterface with IsDead being true
	//		- Without TargetLockableComponent

	AActor* Owner = GetOwner();

	FCollisionQueryParams SweepQueryParams;
	SweepQueryParams.AddIgnoredActor(Owner);
	SweepQueryParams.bIgnoreBlocks = true;

	const float SearchDistance = TargetLockingParams.SearchRadius;
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SearchDistance);

	FVector SearchOrigin;
	FRotator _Rotator;
	Owner->GetActorEyesViewPoint(SearchOrigin, _Rotator);

	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByChannel(Hits, SearchOrigin, SearchOrigin, FQuat::Identity, TargetLockingParams.TargetLockableChannel, CollisionShape, SweepQueryParams);

	if (DEBUG_ENABLED)
	{
		DrawDebugSphere(GetWorld(), Owner->GetActorLocation(), SearchDistance, 12, FColor::Red, false, 5.f);
		PrintToScreen(FString::Printf(TEXT("TargetLockingComponent: Found %d Hits"), Hits.Num()));
	}

	if (Hits.Num() == 0)
	{
		return;
	}

	TSet<AActor*> Actors;
	for (const FHitResult& Hit : Hits)
	{
		Actors.Add(Hit.GetActor());
	}

	// --- Filtering Params
	FVector Forward = Camera->GetForwardVector();
	FVector SelfLocation = Owner->GetActorLocation();

	FCollisionQueryParams LineTraceQueryParams;
	LineTraceQueryParams.AddIgnoredActor(Owner);

	const bool bIsInCombat = GameplaySystem->HasTag(GAMEPLAYTAG_Status_CombatReady_InCombat);

	FHitResult Hit;

	for (AActor* Actor : Actors)
	{
		// Forward direction
		{
			FVector HitDirection = Actor->GetActorLocation() - SelfLocation;
			HitDirection.Normalize();

			float DotProduct = FVector::DotProduct(HitDirection, Forward);
			if (DotProduct < TargetLockingParams.ThetaLimit)
			{
				continue;
			}
		}

		// LOS
		{
			const bool bWasBlocked = GetWorld()->LineTraceSingleByChannel(Hit, SearchOrigin, Actor->GetActorLocation(), ECollisionChannel::ECC_Visibility, LineTraceQueryParams);

			if (DEBUG_ENABLED)
			{
				DrawDebugLine(GetWorld(), SearchOrigin, Hit.Location, FColor::Yellow, false, 5.f);
			}

			if (bWasBlocked)
			{
				continue;
			}
		}

		// Is alive
		{
			// Non damageables might want to use the component in the future, so we only filter out IsDead if they implement the interface
			if (Actor->Implements<UReceiveDamageEventInterface>())
			{
				const bool bIsDead = IReceiveDamageEventInterface::Execute_IsDead(Actor);
				if (bIsDead)
				{
					continue;
				}
			}
		}

		// TargetLockableComponent
		{
			UTargetLockableComponent* TargetLockableComp = Actor->GetComponentByClass<UTargetLockableComponent>();
			if (!TargetLockableComp)
			{
				continue;
			}

			if (TargetLockableComp == Ignore)
			{
				continue;
			}

			if (bIsInCombat && !TargetLockableComp->IsLockableInCombat())
			{
				continue;
			}

			if (!TargetLockableComp->IsLockable())
			{
				continue;
			}


			// Passed all filtering, add it
			OutComponents.Add(TargetLockableComp);
		}
	}
}

void UTargetLockingComponent::FindWarpableActors(float Radius, TArray<AActor*>& OutActors, AActor* Ignore)
{
	// We filter out Hits that are:
	//		- Not in the MotionWarpable channel
	//		- Non-unique or blocking
	//		- Not in LOS
	//		- Implement IReceiveDamageEventInterface with IsDead being true

	AActor* Owner = GetOwner();

	FCollisionQueryParams SweepQueryParams;
	SweepQueryParams.AddIgnoredActor(Owner);
	if (Ignore)
	{
		SweepQueryParams.AddIgnoredActor(Ignore);
	}

	SweepQueryParams.bIgnoreBlocks = true;

	const float SearchDistance = Radius;
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SearchDistance);

	FVector SearchOrigin;
	FRotator _Rotator;
	Owner->GetActorEyesViewPoint(SearchOrigin, _Rotator);

	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByChannel(Hits, SearchOrigin, SearchOrigin, FQuat::Identity, MotionWarpableChannel, CollisionShape, SweepQueryParams);

	if (DEBUG_ENABLED)
	{
		DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), SearchDistance, 12, FColor::Red, false, 5.0f);
		PrintToScreen(FString::Printf(TEXT("TargetLockingComponent: Found %d Warp Targets"), Hits.Num()));
	}

	if (Hits.Num() == 0)
	{
		return;
	}

	TSet<AActor*> Actors;
	for (const FHitResult& Hit : Hits)
	{
		Actors.Add(Hit.GetActor());
	}

	// --- Filtering Params
	const FVector Forward = Owner->GetActorForwardVector();
	const FVector SelfLocation = Owner->GetActorLocation();

	FCollisionQueryParams LineTraceQueryParams;
	LineTraceQueryParams.AddIgnoredActor(Owner);

	FHitResult Hit;

	for (AActor* Actor : Actors)
	{
		// LOS
		{
			const bool bWasBlocked = GetWorld()->LineTraceSingleByChannel(Hit, SearchOrigin, Actor->GetActorLocation(), ECollisionChannel::ECC_Visibility, LineTraceQueryParams);

			if (DEBUG_ENABLED)
			{
				DrawDebugLine(GetWorld(), SearchOrigin, Hit.Location, FColor::Yellow, false, 5.f);
			}

			if (bWasBlocked)
			{
				continue;
			}
		}

		// Is alive
		{
			// Non damageables might want to use the component in the future, so we only filter out IsDead if they implement the interface
			if (Actor->Implements<UReceiveDamageEventInterface>())
			{
				const bool bIsDead = IReceiveDamageEventInterface::Execute_IsDead(Actor);
				if (bIsDead)
				{
					continue;
				}
			}
		}

		// Passed all filtering, add it
		OutActors.Add(Actor);
	}
}

void UTargetLockingComponent::ReceiveRewardMessage(FGameplayTag Channel, const FAARewardMessage& Message)
{
	if (Message.Target == GetTargetedActor())
	{
		StopLocking();
	}
}

void UTargetLockingComponent::ReceiveVerbMessage(FGameplayTag Channel, const FAAVerbMessage& Message)
{
	if (Message.Verb == GAMEPLAYTAG_Verbs_StopTargetLock)
	{
		if (Message.Instigator == GetTargetedActor())
		{
			StopLocking();

			// See if we can lock onto another enemy
			TryLock();
		}
	}
}

void UTargetLockingComponent::InitializeTargetLocking()
{
	WidgetComponent = CreateTargetLockingWidget();

	check(WidgetComponent);

	WidgetComponent->SetVisibility(false /* bNewVisibility */, true /* bPropagateToChildren */);

	UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(this);

	{
		FGameplayMessageListenerParams<FAARewardMessage> Params;
		Params.MatchType = EGameplayMessageMatch::ExactMatch;
		Params.SetMessageReceivedCallback(this, &UTargetLockingComponent::ReceiveRewardMessage);

		RewardHandle = MessageSystem->RegisterListener(GAMEPLAYTAG_Channels_Combat_Rewards, Params);
	}
	{
		FGameplayMessageListenerParams<FAAVerbMessage> Params;
		Params.MatchType = EGameplayMessageMatch::ExactMatch;
		Params.SetMessageReceivedCallback(this, &UTargetLockingComponent::ReceiveVerbMessage);
		VerbHandle = MessageSystem->RegisterListener(GAMEPLAYTAG_Channels_GameplayEvents, Params);
	}
}

bool UTargetLockingComponent::IsWarpTargetBlocked(FVector Target, FVector& OutLocation)
{
	OutLocation = FVector::ZeroVector;

	if (AActor* Owner = GetOwner())
	{
		UWorld* World = GetWorld();
		const FVector OwnerLocation = Owner->GetActorLocation();

		FCollisionQueryParams LineTraceQueryParams;
		LineTraceQueryParams.AddIgnoredActor(Owner);

		FHitResult Result;
		const bool bBlocked = World->LineTraceSingleByProfile(Result, OwnerLocation, Target, TargetLockingParams.BodyCollisionProfile, LineTraceQueryParams);

		OutLocation = Result.Location;

		return bBlocked;
	}

	return false;
}