// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Level/TraversablePath.h"
#include "AIController.h"

int32 FPathFollowerParams::GetDirection() const
{
	return Direction == EPathDirection::EPD_Forward ? 1 : -1;
}

void FPathFollowerParams::ReverseDirection()
{
	switch (Direction)
	{
	case EPathDirection::EPD_Forward:
		Direction = EPathDirection::EPD_Backwards;
		break;

	case EPathDirection::EPD_Backwards:
		Direction = EPathDirection::EPD_Forward;
		break;
		
	default:
		checkNoEntry(); // Type not supported yet
	}
}

FPathFollowerData::FPathFollowerData(AActor* InFollower, const FPathFollowerParams& InParams, ATraversablePath* InPath)
{
	check(InPath);

	Follower = MakeWeakObjectPtr(InFollower);
	Params = InParams;
	OwnerPath = InPath;
}

void FPathFollowerData::Initialize()
{
	TryGetController();

	if (Params.bStartAtClosestPoint)
	{
		int32 StartPoint = OwnerPath->GetClosestPoint(Follower->GetActorLocation());
		ensure(StartPoint != INVALID_POINT);

		CurrentPoint = StartPoint;
	}

	if (UPathFollowingComponent* PathFollowingComp = GetPathFollowingComponent())
	{
		// We want to bind through our owner object, since raw pointers will be invalid if our owning container is resized
		PathFollowingComp->OnRequestFinished.AddUObject(OwnerPath, &ATraversablePath::OnMoveComplete);
	}

	MoveToNextPoint();
}

void FPathFollowerData::TryGetController()
{
	if (AActor* DerefFollower = Follower.Get())
	{
		if (APawn* Pawn = Cast<APawn>(DerefFollower))
		{
			AController* Controller = Pawn->GetController();

			AAIController* NewAIController = Cast<AAIController>(Controller);

			if (NewAIController)
			{
				bHasController = true;
				AIController = MakeWeakObjectPtr(NewAIController);
			}
		}
	}
}

UPathFollowingComponent* FPathFollowerData::GetPathFollowingComponent() const
{
	if (AAIController* DerefAIController = AIController.Get())
	{
		return AIController->GetPathFollowingComponent();
	}

	return nullptr;
}

void FPathFollowerData::Tick(float DeltaTime)
{
	if (Follower.IsExplicitlyNull())
	{
		MarkForRemoval();
		return;
	}

	if (bPaused || bPendingRemove)
	{
		return;
	}

	DelayProgress += DeltaTime;

	if (HasActiveDelay())
	{
		return;
	}

	MoveDelay = 0.0f;
	DelayProgress = 0.0f;

	// Delays for non-controllers also mean that we should avoid checking progress
	if (ShouldCheckProgress())
	{
		if (HasArrivedAtNextPoint())
		{
			IncrementPoint();
		}
	}
	else // Delays for controllers mean that we should wait before trying to navigate again
	{
		MoveToNextPoint();
	}
}

bool FPathFollowerData::HasArrivedAtNextPoint() const
{
	if (AActor* DerefFollower = Follower.Get())
	{
		const FVector TargetLocation = OwnerPath->GetPoint(CurrentPoint).Position;
		float Distance = (DerefFollower->GetActorLocation() - TargetLocation).Length();

		return Distance <= Params.AcceptanceRadius;
	}

	return false;
}

bool FPathFollowerData::HasActiveDelay() const
{
	return MoveDelay > DelayProgress;
}

void FPathFollowerData::IncrementPoint()
{
	const int32 NextPoint = CurrentPoint + Params.GetDirection();
	const int32 TotalPoints = OwnerPath->Path->GetNumberOfSplinePoints();
	const bool bPastEnd = NextPoint >= TotalPoints;
	const bool bBeforeStart = NextPoint < 0;

	MoveDelay += Params.PointReachedWaitTime;

	if (bBeforeStart || bPastEnd)
	{
		if (!Params.bRetraceWhenFinished)
		{
			Params.OnReachedEndDelegate.ExecuteIfBound();
			bPendingRemove = true;
			return;
		}
		
		Params.ReverseDirection();
		CurrentPoint += Params.GetDirection();

		Params.OnReachedPointDelegate.ExecuteIfBound();
		return;
	}

	CurrentPoint = NextPoint;
	Params.OnReachedPointDelegate.ExecuteIfBound();
}

void FPathFollowerData::MoveToNextPoint()
{
	if (Params.bAutoPath; AAIController* Controller = AIController.Get())
	{
		const FVector Target = OwnerPath->GetPoint(CurrentPoint).Position;
		FAIMoveRequest Request = { Target };

		Request.SetAcceptanceRadius(Params.AcceptanceRadius);
		
		FPathFollowingRequestResult Results = Controller->MoveTo(Request);
		CurrentMovement = Results.MoveId;
	}
}

bool FPathFollowerData::ShouldCheckProgress() const
{
	return !bHasController;
}

void FPathFollowerData::MarkForRemoval()
{
	bPendingRemove = true;

	// Unbinds any delegates and dependencies
	PausePathing();
}

void FPathFollowerData::PausePathing()
{
	if (bPaused)
	{
		return;
	}

	if (UPathFollowingComponent* PathFollowingComp = GetPathFollowingComponent())
	{
		PathFollowingComp->OnRequestFinished.RemoveAll(OwnerPath);
		PathFollowingComp->PauseMove(CurrentMovement, EPathFollowingVelocityMode::Reset);
	}

	MoveDelay = 0.0f;
	bPaused = true;
}

void FPathFollowerData::ResumePathing()
{
	if (!bPaused)
	{
		return;
	}

	if (UPathFollowingComponent* PathFollowingComp = GetPathFollowingComponent())
	{
		// We want to bind through our owner object, since raw pointers will be invalid if our owning container is resized
		PathFollowingComp->OnRequestFinished.AddUObject(OwnerPath, &ATraversablePath::OnMoveComplete);
	}

	bPaused = false;
}

void FPathFollowerData::ReceivePathingResults(const FPathFollowingResult& Results)
{
	if (Results.IsSuccess())
	{
		if (bHasController)
		{
			IncrementPoint();
		}

		return;
	}

	if (Results.IsFailure())
	{
		MoveDelay += RETRY_DELAY;
		return;
	}

	if (Results.IsInterrupted())
	{
		// This shouldn't happen if we block off other AI logic properly
		ensureAlways(false);
		return;
	}
}


FPathFollowerHandle::FPathFollowerHandle(uint8 InId)
{
	Id = InId;
}

inline FPathFollowerHandle FPathFollowerHandle::MakeInvalidHandle()
{
	return FPathFollowerHandle(INVALID_PATH_FOLLOWER_HANDLE_ID);
}

ATraversablePath::ATraversablePath(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CustomRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Custom Root"));
	CustomRoot->SetMobility(EComponentMobility::Static);
	SetRootComponent(CustomRoot);

	Path = ObjectInitializer.CreateDefaultSubobject<USplineComponent>(this, TEXT("Path"));
	Path->SetMobility(EComponentMobility::Static);
	Path->SetupAttachment(CustomRoot);
}

void ATraversablePath::BeginPlay()
{
	Super::BeginPlay();
}

void ATraversablePath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (auto& [Handle, PathFollower] : PathFollowers)
	{
		PathFollower.Tick(DeltaTime);
	}
}

bool ATraversablePath::CanBeValidHandle(const FPathFollowerHandle& Handle)
{
	return Handle.Id != INVALID_PATH_FOLLOWER_HANDLE_ID;
}

bool ATraversablePath::IsValidHandle(const FPathFollowerHandle& Handle) const
{
	if (!CanBeValidHandle(Handle))
	{
		return false;
	}

	return PathFollowers.Contains(Handle);
}

FPathFollowerHandle ATraversablePath::StartTraversing(AActor* TraversingActor)
{
	check(TraversingActor);

	return StartTraversing_Params(TraversingActor, DefaultParams);
}

FPathFollowerHandle ATraversablePath::StartTraversing_Params(AActor* TraversingActor, const FPathFollowerParams& Params)
{
	check(TraversingActor);

	FPathFollowerHandle Handle = GetHandleForActor(TraversingActor);

	if (IsValidHandle(Handle))
	{
		ResumeTraversing(Handle);

		return Handle;
	}

	return StartTraversing_Internal(TraversingActor, Params);
}

FPathFollowerHandle ATraversablePath::StartTraversing_DelegatesOnly(AActor* TraversingActor, const FOnReachedPointSignature& OnReachedPointDelegate, const FOnReachedEndSignature& OnReachedEndDelegate)
{
	check(TraversingActor);

	FPathFollowerParams DefaultsCopy = DefaultParams;
	DefaultsCopy.OnReachedPointDelegate = OnReachedPointDelegate;
	DefaultsCopy.OnReachedEndDelegate = OnReachedEndDelegate;

	return StartTraversing_Params(TraversingActor, DefaultsCopy);
}

bool ATraversablePath::ResumeTraversing(const FPathFollowerHandle& Handle)
{
	if (FPathFollowerData* DataPtr = PathFollowers.Find(Handle))
	{
		DataPtr->ResumePathing();
		return true;
	}

	return false;
}

void ATraversablePath::StopTraversing(const FPathFollowerHandle& Handle, bool bEraseData)
{
	if (FPathFollowerData* DataPtr = PathFollowers.Find(Handle))
	{
		if (bEraseData)
		{
			DataPtr->MarkForRemoval();
		}
		else
		{
			DataPtr->PausePathing();
		}
	}
}

void ATraversablePath::OnMoveComplete(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	for (auto& [Handle, PathFollower] : PathFollowers)
	{
		if (PathFollower.CurrentMovement == RequestID)
		{
			PathFollower.ReceivePathingResults(Result);

			return;
		}
	}
}

FVector ATraversablePath::GetNextPoint(const FPathFollowerHandle& Handle)
{
	if (FPathFollowerData* DataPtr = PathFollowers.Find(Handle))
	{
		FSplinePoint Point = GetPoint(DataPtr->CurrentPoint);
		return Point.Position;
	}

	return FVector();
}

int32 ATraversablePath::GetClosestPoint(FVector WorldLocation)
{
	const int32 Points = Path->GetNumberOfSplinePoints();

	if (Points == 0)
	{
		return INVALID_POINT;
	}

	int32 ClosestPoint = 0;
	float ClosestDistance = FLT_MAX;
	for (int32 i = 0; i < Points; i++)
	{
		FSplinePoint SplinePoint = GetPoint(i);

		float Distance = (SplinePoint.Position - WorldLocation).Length();
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestPoint = i;
		}
	}

	return ClosestPoint;
}

FPathFollowerHandle ATraversablePath::GetHandleForActor(AActor* TraversingActor)
{
	check(TraversingActor);

	for (const auto& [Handle, PathFollower] : PathFollowers)
	{
		if (PathFollower.Follower == TraversingActor)
		{
			return Handle;
		}
	}

	return FPathFollowerHandle::MakeInvalidHandle();
}

FPathFollowerHandle ATraversablePath::StartTraversing_Internal(AActor* TraversingActor, const FPathFollowerParams& Params)
{
	check(TraversingActor);

	FPathFollowerData Data = FPathFollowerData(TraversingActor, Params, this);
	Data.Initialize();

	FPathFollowerHandle NewHandle = MakeNewHandle();

	PathFollowers.Emplace(NewHandle, Data);



	return NewHandle;
}

FSplinePoint ATraversablePath::GetPoint(int32 Index)
{
	FSplinePoint Point = {};

	if (!Path)
	{
		return Point;
	}

	if (Path->GetNumberOfSplinePoints() < Index)
	{
		return Point;
	}

	Point = Path->GetSplinePointAt(Index, ESplineCoordinateSpace::World);

	// GetSplinePointAt does in fact not adjust the position of the point based on ESplineCoordinateSpace, despite doing it in GetLocationAtSplinePoint! Stupid!
	Point.Position = Path->GetComponentTransform().TransformPosition(Point.Position);

	return Point;
}

FPathFollowerHandle ATraversablePath::MakeNewHandle()
{
	return FPathFollowerHandle(NextFollowerId++);
}

uint32 GetTypeHash(const FPathFollowerHandle& InHandle)
{
	return GetTypeHash(InHandle.Id);
}
