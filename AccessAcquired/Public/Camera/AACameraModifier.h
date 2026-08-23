// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "Camera/CameraTypes.h"
#include "AACameraModifier.generated.h"

// Original design is sourced from Daedelic Entertainment at https://github.com/DaedalicEntertainment/third-person-camera/tree/develop
// Further iterated, expanded and improved on for Access Acquired.

USTRUCT(BlueprintType)
struct FAACameraInfo
{
    GENERATED_BODY()

    // The field of view when the camera is in Perspective mode.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Degrees"), Category = "Camera")
    float FOV = 0.0f;

    // Distance between the camera and the view target.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Centimeters"), Category = "Camera")
    float SpringArmLength = 0.0f;

    // Distance in which the Depth of Field effect should be sharp.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Centimeters"), Category = "Camera|DepthOfField")
    float DepthOfFieldFocalDistance = 0.0f;

    // Artificial region where all content is in focus, starting after DepthOfFieldFocalDistance.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Centimeters"), Category = "Camera|DepthOfField")
    float DepthOfFieldFocalRegion = 0.0f;

    // To define the width of the transition region next to the focal region on the near side.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Centimeters"), Category = "Camera|DepthOfField")
    float DepthOfFieldNearTransitionRegion = 0.0f;

    // To define the width of the transition region next to the focal region on the near side.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Centimeters"),  Category = "Camera|DepthOfField")
    float DepthOfFieldFarTransitionRegion = 0.0f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Offsets")
    FVector SocketOffset = FVector::ZeroVector;

    // Evaluates the curve with Alpha, and uses the curves value instead. 
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Offsets")
    TObjectPtr<UCurveFloat> AlphaCurve = nullptr;

    bool Equals(const FAACameraInfo& Other) const;

    // If this CameraInfo is meaningful, or modifies no properties.
    bool IsZero() const;
};

/**
 * 
 */
UCLASS(Blueprintable)
class VERTICALSLICE_API UAACameraModifier : public UCameraModifier
{
	GENERATED_BODY()

public:
    UAACameraModifier(const FObjectInitializer& ObjectInitializer);

    virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

    virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

    // Gets the camera modifiers that are currently being applied.
    FAACameraInfo GetCurrentModifiers() const;

    // Gets the camera modifiers that we are currently transitioning to.
    FAACameraInfo GetTargetModifiers() const;

    // Gradually applies a transition between two different camera modifiers.
    void ApplyCameraTransition(const FAACameraInfo& CameraInfo, FMinimalViewInfo& OutPOV, const float DeltaTime);

protected:

    void ApplyCameraInfo(const FAACameraInfo& CameraInfo, FMinimalViewInfo& OutPOV) const;

    // The modifiers that this CameraModifier wants to apply
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AACameraModifier")
    FAACameraInfo AppliedModifiers = FAACameraInfo();

    FAACameraInfo CurrentModifiers = FAACameraInfo();

    FAACameraInfo TargetModifiers = FAACameraInfo();
};
