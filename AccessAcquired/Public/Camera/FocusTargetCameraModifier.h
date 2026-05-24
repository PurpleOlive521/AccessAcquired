// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "FocusTargetCameraModifier.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UFocusTargetCameraModifier : public UCameraModifier
{
	GENERATED_BODY()

public:
	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

	UPROPERTY(BlueprintReadWrite, Category = "DroneBoundCameraModifier")
	TObjectPtr<AActor> OverrideTarget = nullptr;

protected:

	// The maximum amount of degrees the camera can pitch downwards to a Target. Useful to limit the angle when the camera is very close to a Target.
	UPROPERTY(EditAnywhere, Category = "DroneBoundCameraModifier")
	float MinimumAllowedPitch = 25.0f;

	// The speed at which the camera rotates towards a Target.
	UPROPERTY(EditAnywhere, Category = "DroneBoundCameraModifier")
	float SlewSpeed = 15.0f;
};
