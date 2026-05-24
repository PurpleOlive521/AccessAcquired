// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "DroneBoundCameraModifier.generated.h"

class USpringArmComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UDroneBoundCameraModifier : public UCameraModifier
{
	GENERATED_BODY()

public:

	virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;

	FORCEINLINE FVector GetTargetOffset(float Offset) const;

	void ApplyCameraOffsets(USpringArmComponent& SpringArm, float DeltaTime) const;

protected:

	bool bUsingSocketOffset = false;

	float LastUsedOffset = 150.0f;

	float TargetOffsetLerpAlpha = 0.0f;

	FVector TargetOffset = FVector::ZeroVector;

	bool bIsOnTheRight = false;

	bool bIsOnTheLeft = false;
};
