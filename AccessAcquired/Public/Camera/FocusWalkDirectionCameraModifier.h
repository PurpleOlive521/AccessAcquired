// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "FocusWalkDirectionCameraModifier.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UFocusWalkDirectionCameraModifier : public UCameraModifier
{
	GENERATED_BODY()
	
public:
	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

protected:

	UPROPERTY(EditAnywhere, Category = "FocusWalkDirectionCameraModifier")
	float MaxInterpolation = 0.20f;

	UPROPERTY(EditAnywhere, Category = "FocusWalkDirectionCameraModifier")
	bool bIncreaseInterpolationWhenRunning = true;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bIncreaseInterpolationWhenRunning == true"), Category = "FocusWalkDirectionCameraModifier")
	float AddedInterpolationWhenRunning = 0.20f;

	// Lower dot values trigger FocusWalkDirection when facing away from the camera.
	UPROPERTY(EditAnywhere, Category = "FocusWalkDirectionCameraModifier")
	float MinimumAwayDot = 0.90f;

	// Lower dot values trigger FocusWalkDirection when facing towards from the camera.
	UPROPERTY(EditAnywhere, Category = "FocusWalkDirectionCameraModifier")
	float MinimumTowardsDot = 0.50f;
};
