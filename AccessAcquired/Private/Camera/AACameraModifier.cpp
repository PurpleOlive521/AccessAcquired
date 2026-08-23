// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Camera/AACameraModifier.h"
#include "AASpringArmComponent.h"

bool FAACameraInfo::Equals(const FAACameraInfo& Other) const
{
    FAACameraInfo Delta;

    Delta.FOV =                                 FOV - Other.FOV;
    Delta.SpringArmLength =                     SpringArmLength - Other.SpringArmLength;
    Delta.DepthOfFieldFocalDistance =           DepthOfFieldFocalDistance - Other.DepthOfFieldFocalDistance;
    Delta.DepthOfFieldFocalRegion =             DepthOfFieldFocalRegion - Other.DepthOfFieldFocalRegion;
    Delta.DepthOfFieldNearTransitionRegion =    DepthOfFieldNearTransitionRegion - Other.DepthOfFieldNearTransitionRegion;
    Delta.DepthOfFieldFarTransitionRegion =     DepthOfFieldFarTransitionRegion - Other.DepthOfFieldFarTransitionRegion;
    Delta.SocketOffset =                        SocketOffset - Other.SocketOffset;

    return Delta.IsZero();
}

bool FAACameraInfo::IsZero() const
{
    return 
        FMath::Abs(FOV)                                 < KINDA_SMALL_NUMBER && 
        FMath::Abs(SpringArmLength)                     < KINDA_SMALL_NUMBER &&
        FMath::Abs(DepthOfFieldFocalDistance)           < KINDA_SMALL_NUMBER && 
        FMath::Abs(DepthOfFieldFocalRegion)             < KINDA_SMALL_NUMBER &&
        FMath::Abs(DepthOfFieldNearTransitionRegion)    < KINDA_SMALL_NUMBER &&
        FMath::Abs(DepthOfFieldFarTransitionRegion)     < KINDA_SMALL_NUMBER &&
        SocketOffset.IsNearlyZero();
}

UAACameraModifier::UAACameraModifier(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool UAACameraModifier::ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
    Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

    return false;
}

bool UAACameraModifier::ModifyCamera(float DeltaTime, FMinimalViewInfo& OutPOV)
{
    Super::ModifyCamera(DeltaTime, OutPOV);

    if (AppliedModifiers.IsZero())
    {
        return false;
    }

    ApplyCameraTransition(AppliedModifiers, OutPOV, DeltaTime);

    return false;
}

FAACameraInfo UAACameraModifier::GetCurrentModifiers() const
{
    return CurrentModifiers;
}

FAACameraInfo UAACameraModifier::GetTargetModifiers() const
{
    return TargetModifiers;
}

void UAACameraModifier::ApplyCameraTransition(const FAACameraInfo& CameraInfo, FMinimalViewInfo& OutPOV, const float DeltaTime)
{
    if (!TargetModifiers.Equals(CameraInfo))
    {
        CurrentModifiers = TargetModifiers;
        TargetModifiers = CameraInfo;
    }

    if (CurrentModifiers.Equals(TargetModifiers))
    {
        Alpha = 1.0f;
        ApplyCameraInfo(CurrentModifiers, OutPOV);
        return;
    }

    FAACameraInfo ModifiersToApply;

    if (not TargetModifiers.IsZero())
    {
        ModifiersToApply = TargetModifiers;
    }
    else
    {
        ModifiersToApply = CurrentModifiers;
    }

    // Apply modifiers.
    ApplyCameraInfo(ModifiersToApply, OutPOV);
}

void UAACameraModifier::ApplyCameraInfo(const FAACameraInfo& CameraInfo, FMinimalViewInfo& InOutPOV) const
{
    AActor* ViewTarget = GetViewTarget();
    if (not ViewTarget)
    {
        return;
    }

    float EvaluatedAlpha = Alpha;
    
    // If we have a curve to sample from, use it's evaluated alpha instead
    if (CameraInfo.AlphaCurve)
    {
        EvaluatedAlpha = CameraInfo.AlphaCurve->GetFloatValue(Alpha);
    }

    float AppliedFOV = CameraInfo.FOV * EvaluatedAlpha;
    InOutPOV.FOV = InOutPOV.FOV + AppliedFOV;

    if (UAASpringArmComponent* SpringArmComponent = ViewTarget->GetComponentByClass<UAASpringArmComponent>())
    {
        SpringArmComponent->TargetArmLengthModifier += CameraInfo.SpringArmLength * EvaluatedAlpha;
        SpringArmComponent->SocketOffsetModifier += CameraInfo.SocketOffset * EvaluatedAlpha;
    }

    // Apply depth of field.
    if (not FMath::IsNearlyZero(CameraInfo.DepthOfFieldFocalDistance))
    {
        InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
        InOutPOV.PostProcessSettings.DepthOfFieldFocalDistance += CameraInfo.DepthOfFieldFocalDistance * EvaluatedAlpha;
    }

    if (not FMath::IsNearlyZero(CameraInfo.DepthOfFieldFocalRegion))
    {
        InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFocalRegion = true;
        InOutPOV.PostProcessSettings.DepthOfFieldFocalRegion += CameraInfo.DepthOfFieldFocalRegion * EvaluatedAlpha;
    }

    if (not FMath::IsNearlyZero(CameraInfo.DepthOfFieldNearTransitionRegion))
    {
        InOutPOV.PostProcessSettings.bOverride_DepthOfFieldNearTransitionRegion = true;
        InOutPOV.PostProcessSettings.DepthOfFieldNearTransitionRegion += CameraInfo.DepthOfFieldNearTransitionRegion * EvaluatedAlpha;
    }

    if (not FMath::IsNearlyZero(CameraInfo.DepthOfFieldFarTransitionRegion))
    {
        InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFarTransitionRegion = true;
        InOutPOV.PostProcessSettings.DepthOfFieldFarTransitionRegion += CameraInfo.DepthOfFieldFarTransitionRegion * EvaluatedAlpha;
    }
}