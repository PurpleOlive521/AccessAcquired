// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Camera/CameraComponent.h"

#include "TargetLockingComponent.generated.h"

class UMotionWarpingComponent;


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VERTICALSLICE_API UTargetLockingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTargetLockingComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = " TargetLocking")
	void RequestTargetLock(bool bDebug = false);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = " TargetLocking")
	void RequestSwitchTarget(FVector2D SwitchDirection, bool bDebug = false);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TargetLocking")
	AActor* GetTargetedActor();

	/** Attempts to find a Enemy to lock the camera on, keeping the enemy on the screen by turning the camera independently of the player.
	* If no enemies are found, nothing is done. Returns true if an target is found, false if not. */
	UFUNCTION(BlueprintCallable, Category = "TargetLocking")
	bool TryLockOntoActor(float SearchRadius, FVector SearchOrigin, bool bDebug = false);

	/** Attempts to switch to another eligible target in the SwitchDirection, relative to screenspace.
	* Returns true if switched to a new target, and false if it stays on the current target. */
	UFUNCTION(BlueprintCallable, Category = "TargetLocking")
	bool SwitchLockedActor(FVector2D SwitchDirection, float SearchRadius, FVector SearchOrigin, AActor*& PreviousTarget, bool bDebug = false);

	// For unlocking when the targeted Enemy is dead, and attempting to reacquire lock on another Enemy if present.
	UFUNCTION(BlueprintCallable, Category = "TargetLocking")
	void OnEnemyDied(AActor* Enemy);

	// --- Motion Warping
	
	// Warps the Actor towards a found Target. If no Targets are found, it instead warps in the Actors current direction
	// CameraRelativeInput is used as the direction instead of the Targets direction if given.
	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	void WarpToClosestTarget(FVector CameraRelativeInput, bool bDebug = false);

	// Warp towards a specific Actor, warping either the full distance or up to the limit set by WarpSearchDistance.
	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	void WarpTowardsActor(AActor* Actor, bool bDebug = false);

	// Allows adjustments of the warp targets 
	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	void UpdateWarpRotation(FVector CameraRelativeDirection, bool bDebug = false);

	// Removes all WarpTargets from the MotionWarpingComponent
	UFUNCTION(BlueprintCallable, Category = "MotionWarping")
	void ClearWarpTargets();

protected:
	// Overrides Control Rotation to keep the camera centered on the target
	UFUNCTION(BlueprintCallable, Category = "TargetLocking")
	void KeepCameraOnTarget(float DeltaTime);

	// The maximum angle a Target can be off from Players camera view before it gets ignored as a potential Target
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLocking")
	float ThetaLimit = 0.5f;

	// The maximum amount of degrees the camera can pitch downwards to a Target. Useful to limit the angle when a Target is very close to the player.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLocking")
	float MinimumAllowedPitch = 25.0f;

	// The speed at which the camera moves towards a Target.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLocking")
	float CameraSlewingSpeed = 15.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TargetLocking")
	float SearchDistance = 1500.0f;

	UPROPERTY(BlueprintReadWrite, Category = "TargetLocking")
	TEnumAsByte<ECollisionChannel> LockableActorChannel;

	UPROPERTY(BlueprintReadWrite, Category = "TargetLocking")
	bool bIsLockedOntoTarget = false;

	UPROPERTY(BlueprintReadWrite, Category = "TargetLocking")
	bool bCanSwitchTarget = true;

	UPROPERTY(BlueprintReadWrite, Category = "MotionWarping")
	FName WarpTargetName;

	UPROPERTY(BlueprintReadWrite, Category = "MotionWarping")
	TEnumAsByte<ECollisionChannel> MotionWarpableChannel;

	// The distance the Actor will search for a Target to MotionWarp to.
	UPROPERTY(BlueprintReadOnly, Category = "MotionWarping")
	float WarpSearchDistance = 200.0f;

	// The distance the Actor gets moved forward when nothing to Target is nearby. Interpolated based on current speed.
	// Allows for momentum to be transferred into animations rather than a sudden 100 to 0 stop.
	UPROPERTY(BlueprintReadOnly, Category = "MotionWarping")
	float WarpForwardDistance = 300.0f;

private:

	// Asserts if the component can not be found
	UMotionWarpingComponent* GetMotionWarpingComponent();

	// Do not use directly, instead use GetMotionWarpingComponent()!
	TObjectPtr<UMotionWarpingComponent> MotionWarping;

	AActor* TargetedActor;

	TObjectPtr<UCameraComponent> Camera;

};
