// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/SplineComponent.h"
#include "TraversablePath.generated.h"

class USplineComponent;
class AAIController;
class ATraversablePath;

constexpr int32 INVALID_POINT = -1;
constexpr float RETRY_DELAY = 0.25f;

UENUM(BlueprintType)
enum class EPathDirection : uint8
{
	EPD_Forward		UMETA(DisplayName = "Forward"),
	EPD_Backwards	UMETA(DisplayName = "Backwards"),
};

DECLARE_DYNAMIC_DELEGATE(FOnReachedPointSignature);
DECLARE_DYNAMIC_DELEGATE(FOnReachedEndSignature);

USTRUCT(BlueprintType)
struct FPathFollowerParams
{
	GENERATED_BODY()

	FPathFollowerParams() = default;

	// Returns -1 if EPD_Backwards, 1 if EPD_Forward.
	int32 GetDirection() const;

	void ReverseDirection();
	
	// The direction we traverse the path in.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPathDirection Direction = EPathDirection::EPD_Forward;

	// Will reverse direction and backtrack once the end of the path is hit.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bRetraceWhenFinished = false;

	// Will find the closest point on the path and start traversal from there.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bStartAtClosestPoint = false;

	// Will send MoveTo commands to the AIController whenever the next point is reached.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAutoPath = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AcceptanceRadius = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PointReachedWaitTime = 0.0f;

	// Any bound function is called when the next point is reached.
	UPROPERTY(BlueprintReadWrite)
	FOnReachedPointSignature OnReachedPointDelegate;

	// Any bound function is called when the last point is reached. Not called if bRetraceWhenFinished is true!
	UPROPERTY(BlueprintReadWrite)
	FOnReachedEndSignature OnReachedEndDelegate;
};

struct FPathFollowerData
{
	FPathFollowerData() = delete;

	explicit FPathFollowerData(AActor* InFollower, const FPathFollowerParams& InParams, ATraversablePath* InPath);

	void Initialize();

	void TryGetController();

	// Can return nullptr.
	UPathFollowingComponent* GetPathFollowingComponent() const;

	void Tick(float DeltaTime);

	bool HasArrivedAtNextPoint() const;

	// An active delay prohibits movement commands for the duration of the delay
	bool HasActiveDelay() const;

	// Starts pathing to the next point.
	void IncrementPoint();

	// Calls the MoveTo function on the AIController, if one exists.
	void MoveToNextPoint();

	// Actors with AIControllers will receive progress updates through the PathFollowerComponent rather than manual checks.
	bool ShouldCheckProgress() const;

	void MarkForRemoval();

	void PausePathing();

	void ResumePathing();

	void ReceivePathingResults(const FPathFollowingResult& Results);

	FPathFollowerParams Params;

	TWeakObjectPtr<AActor> Follower = nullptr;

	TWeakObjectPtr<AAIController> AIController = nullptr;

	TObjectPtr<ATraversablePath> OwnerPath = nullptr;

	FAIRequestID CurrentMovement = FAIRequestID();

	float MoveDelay = 0.0f;

	float DelayProgress = 0.0f;

	int32 CurrentPoint = 0;

	bool bPaused = false;

	bool bHasController = false;

private:
	bool bPendingRemove = false;
};

constexpr uint8 INVALID_PATH_FOLLOWER_HANDLE_ID = 0U;

USTRUCT(BlueprintType)
struct VERTICALSLICE_API FPathFollowerHandle
{
	GENERATED_BODY()

	FPathFollowerHandle() = default;

	FPathFollowerHandle(uint8 InId);

	inline static FPathFollowerHandle MakeInvalidHandle();

	friend uint32 GetTypeHash(const FPathFollowerHandle& InHandle);

	bool operator==(const FPathFollowerHandle& Other) const
	{
		return Id == Other.Id;
	}

	bool operator!=(const FPathFollowerHandle& Other) const
	{
		return Id != Other.Id;
	}

	uint8 Id = INVALID_PATH_FOLLOWER_HANDLE_ID;
};

UCLASS()
class VERTICALSLICE_API ATraversablePath : public AActor
{
	GENERATED_BODY()

	friend struct FPathFollowerData;
	
public:	
	ATraversablePath(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// Returns true if the Handle can ever represent a valid PathFollower. Does not ensure that it still is valid.
	UFUNCTION(BlueprintCallable)
	static bool CanBeValidHandle(const FPathFollowerHandle& Handle);

	// Returns true if the Handle is valid in this TraversablePath Actor.
	UFUNCTION(BlueprintCallable)
	bool IsValidHandle(const FPathFollowerHandle& Handle) const;

	// Starts traversal for the Actor with DefaultParams. If a Handle for the Actor exists, it will resume traversal from that data.
	UFUNCTION(BlueprintCallable)
	FPathFollowerHandle StartTraversing(AActor* TraversingActor);

	// Starts traversal for the Actor with the specified Params. If a Handle for the Actor exists, it will resume traversal from that data.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Start Traversing with Params"))
	FPathFollowerHandle StartTraversing_Params(AActor* TraversingActor, const FPathFollowerParams& Params);

	// Starts traversal for the Actor with DefaultParams. If a Handle for the Actor exists, it will resume traversal from that data.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Start Traversing with Delegates"))
	FPathFollowerHandle StartTraversing_DelegatesOnly(AActor* TraversingActor, const FOnReachedPointSignature& OnReachedPointDelegate, const FOnReachedEndSignature& OnReachedEndDelegate);

	// Only resumes if a Handle and previous data exists. Returns true if it could be resumed, false if not.
	UFUNCTION(BlueprintCallable)
	bool ResumeTraversing(const FPathFollowerHandle& Handle);

	// Stop traversing the path. Stops AIController calls from this path. Optionally removes stored data for the Handle.
	UFUNCTION(BlueprintCallable)
	void StopTraversing(const FPathFollowerHandle& Handle, bool bEraseData);

	void OnMoveComplete(FAIRequestID RequestID, const FPathFollowingResult& Result);

	UFUNCTION(BlueprintCallable)
	FVector GetNextPoint(const FPathFollowerHandle& Handle);

	// Returns the index of the point in the SplineComponent. Returns INVALID_POINT if no points exist.
	UFUNCTION(BlueprintCallable)
	int32 GetClosestPoint(FVector WorldLocation);

	// Will return a invalid handle if the Actor has no data associated with it.
	UFUNCTION(BlueprintCallable)
	[[nodiscard]] FPathFollowerHandle GetHandleForActor(AActor* TraversingActor);

protected:

	FPathFollowerHandle StartTraversing_Internal(AActor* TraversingActor, const FPathFollowerParams& Params);

	inline FSplinePoint GetPoint(int32 Index);

	[[nodiscard]] inline FPathFollowerHandle MakeNewHandle();

	UPROPERTY(EditAnywhere, Category = "TraversablePath")
	FPathFollowerParams DefaultParams;

	TMap <FPathFollowerHandle, FPathFollowerData> PathFollowers;

	uint8 NextFollowerId = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TraversablePath")
	TObjectPtr<USceneComponent> CustomRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TraversablePath")
	TObjectPtr<USplineComponent> Path = nullptr;
};
