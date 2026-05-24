// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "MotionWarpingComponent.h"
#include "GameplayMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "TargetLockingComponent.generated.h"

class UGameplaySystemComponent;
class UGameplayEffect;
class UTargetLockableComponent;

// TODO: Move to independent Component separate from MotionWarping functionality?
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FTargetLockingParams
{
	GENERATED_BODY()

	FTargetLockingParams() = default;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLockingParams")
	TSubclassOf<UGameplayEffect> DisableSwitchTargetEffect = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLockingParams")
	float SwitchTargetDeadzone = 0.4f;

	// The maximum angle a Target can be off from Characters camera view before it gets ignored as a potential Target
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLockingParams")
	float ThetaLimit = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLockingParams")
	float IconSlewSpeed = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLockingParams")
	float SearchRadius = 1500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLockingParams")
	TEnumAsByte<ECollisionChannel> TargetLockableChannel = ECollisionChannel::ECC_WorldDynamic;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (GetOptions = "GetCollisionProfiles"), Category = "TargetLockingParams")
	FName BodyCollisionProfile;
};

USTRUCT(BlueprintType)
struct VERTICALSLICE_API FMotionWarpQuery
{
	GENERATED_BODY()

	FMotionWarpQuery() = default;

	// The max distance to the created target.
	// Also the max radius in which we search for existing targets.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MotionWarpQuery")
	float Distance = 50.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MotionWarpQuery")
	bool bScaleBySpeed = true;

	// How much of the speed we apply in the calculation.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MotionWarpQuery")
	float SpeedScalar = 0.25f;
};

UENUM(BlueprintType)
enum class EWarpSource : uint8
{
	// Manual input dictated the source used.
	EWS_OverrideInput	UMETA(DisplayName = "Override Input"),

	// Target Locking targets location was used.
	EWS_TargetLocking	UMETA(DisplayName = "Target Locking"),

	// A target was found dynamically by a manual search.
	EWS_Discovered		UMETA(DisplayName = "Discovered"),
	
	// No target was found.
	EWS_None			UMETA(DisplayName = "None"),
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VERTICALSLICE_API UTargetLockingComponent : public UMotionWarpingComponent
{
	GENERATED_BODY()

public:	
	UTargetLockingComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	
	// TODO: Move to helper object or static function library
	UFUNCTION()
	static TArray<FName> GetCollisionProfiles();

	// --- Target Locking

	// Attempts to find a Target to lock on.
	// Returns true if an target is found and locked on, false if not.
	UFUNCTION(BlueprintCallable, Category = "TargetLocking")
	void TryLock();

	// Attempts to switch to another eligible target in the SwitchDirection, relative to screenspace.
	// Returns true if switched to a new Target, and false if it stays on the current Target.
	UFUNCTION(BlueprintCallable, Category = "TargetLocking")
	void SwitchTarget(FVector2D SwitchDirection);

	UFUNCTION(BlueprintCallable)
	AActor* GetTargetedActor() const;

	UFUNCTION(BlueprintCallable)
	bool IsLockedOntoTarget() const;

	UFUNCTION(BlueprintCallable)
	void LockOnTarget(UTargetLockableComponent* NewTarget);

	UFUNCTION(BlueprintCallable)
	void StopLocking();

	// --- Motion Warping

	// Motion warps to the first valid target found in the order listed:
	// 1. Override inputs, where we move a set distance in the direction of input. For player-controlled this means received movement input.
	// 2. The current TargetLocking actor.
	// 3. The nearest MotionWarpable target that we can find in a set radius.
	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	void TriggerMotionWarp(const FMotionWarpQuery& QueryParams);

	// Finds a Actor that is MotionWarpable to. If multiple candidates exists, the closest one in the Owners forward-direction is chosen.
	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	AActor* FindWarpableTarget(float SearchDistance);

	// Direction must be World-relative direction.
	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	void SetOverrideDirection(FVector2D Direction);

	// The direction for the target to face.
	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	void RotateWarpTarget(FVector2D Direction);

	// The distance from self that the WarpTarget will be placed in a arbituary direction.
	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	float CalculateWarpDistance(const FMotionWarpQuery& QueryParams) const;

	// Removes the WarpTarget used by the TargetLockingComponent only.
	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	void RemoveComponentWarpTarget();

protected: 

	UFUNCTION(BlueprintImplementableEvent, Category = " TargetLocking")
	UWidgetComponent* CreateTargetLockingWidget();

	// Called when we start TargetLocking on a new target.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Start Locking On Target"), Category = " TargetLocking")
	void K2_StartLockingOnTarget();

	// Called when we switch from an existing target to a new one.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Switch Target"), Category = " TargetLocking")
	void K2_SwitchTarget();

	// Called when we stop TargetLocking.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Stop Locking"), Category = " TargetLocking")
	void k2_StopLocking();
	
	bool CanSwitchTarget() const;

	void FindLockableComponents(TArray<UTargetLockableComponent*>& OutComponents, UTargetLockableComponent* Ignore = nullptr);

	void FindWarpableActors(float Radius, TArray<AActor*>& OutActors, AActor* Ignore = nullptr);

	void ReceiveRewardMessage(FGameplayTag Channel, const FAARewardMessage& Message);
	void ReceiveVerbMessage(FGameplayTag Channel, const FAAVerbMessage& Message);

	void InitializeTargetLocking();

	// Returns true if a object with collision is in the way of the Target point.
	// @param OutLocation	The world point where the blocking hit occured (if any).
	bool IsWarpTargetBlocked(FVector Target, FVector& OutLocation);

	// TODO: Temporary solution to avoid extra initialization for Actors that don't require target locking. Split up into two components instead?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLocking")
	bool bEnableTargetLocking = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLocking")
	FTargetLockingParams TargetLockingParams;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MotionWarping")
	FName WarpTargetName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MotionWarping")
	TEnumAsByte<ECollisionChannel> MotionWarpableChannel;

	// The radius at which we can find targets to warp towards, even when not locked on to them.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MotionWarping")
	float DiscoverTargetRadius = 400.0f;

	// The distance from target Actor that we will place the WarpTarget at.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MotionWarping")
	float WarpTargetRadius = 50.0f;

	// The max degrees that a found target can be off from the input direction.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MotionWarping")
	float MaximumAssistedInputDifference = 30.0f;

private:

	// World-space direction
	FVector OverrideDirection = {};

	float CurrentDelta = 0.0f;

	bool bIsLockedOntoTarget = false;

	// TODO: Currently unused. Would be used to determine targets based on Team, so that any Character could use the component.
	ETeam OwnerTeam = ETeam::ET_NotAssigned;

	EWarpSource WarpSource = EWarpSource::EWS_None;
	
	FGameplayMessageListenerHandle RewardHandle;

	FGameplayMessageListenerHandle VerbHandle;

	TObjectPtr<UCameraComponent> Camera = nullptr;

	TObjectPtr<UWidgetComponent> WidgetComponent = nullptr;

	TObjectPtr<UGameplaySystemComponent> GameplaySystem = nullptr;

	TObjectPtr<UTargetLockableComponent> LockedComponent = nullptr;
};
