// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "DroneBoundCameraModifier.h"
#include "Kismet/KismetMathLibrary.h"
#include "TargetLockingComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

void UDroneBoundCameraModifier::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
    Super::ModifyCamera(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);

    AActor* ViewTarget = GetViewTarget();
    if (not ViewTarget)
    {
        return;
    }

    ACharacter* ViewCharacter = Cast<ACharacter>(ViewTarget);
    if (not ViewCharacter)
    {
        return;
    }

    USpringArmComponent* SpringArm = ViewCharacter->GetComponentByClass<USpringArmComponent>();
    if (not SpringArm)
    {
        return;
    }

    const FVector CameraLocation = CameraOwner->GetCameraLocation();
    const FVector ViewTargetLocation = ViewTarget->GetActorLocation();

    // Camera under ViewTarget
    if (CameraLocation.Z < ViewTargetLocation.Z)
    {
        const FVector CameraForward = CameraOwner->GetActorForwardVector();
        const FVector ViewTargetForward = ViewTarget->GetActorForwardVector();
        const FVector ViewTargetRight = ViewTarget->GetActorRightVector();

        const float ForwardAbsDot = fabs(UKismetMathLibrary::Dot_VectorVector(ViewTargetForward, CameraForward));
        const float ForwardAngle = UKismetMathLibrary::DegAcos(ForwardAbsDot);

        const float RightAbsDot = fabs(UKismetMathLibrary::Dot_VectorVector(ViewTargetRight, CameraForward));
        const float RightAngle = UKismetMathLibrary::DegAcos(ForwardAbsDot);

        TargetOffsetLerpAlpha = FMath::Square(RightAngle / 90.0f * ForwardAngle / 90.0f);

        if (bUsingSocketOffset)
        {
            TargetOffset = GetTargetOffset(LastUsedOffset);

            ApplyCameraOffsets(*SpringArm, DeltaTime);
        }
        else
        {
            const float AdjustedZ = ViewCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.5f;
            const FVector Start = ViewTargetLocation - FVector(0.0f, 0.0f, AdjustedZ);

            FCollisionQueryParams Params;
            Params.AddIgnoredActor(ViewTarget);

            {
                FVector End = Start + CameraOwner->GetActorRightVector() * 150.0f;

                FHitResult Hit;
                const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, Params);

                bIsOnTheRight = bBlocked;
            }

            {
                FVector End = Start + CameraOwner->GetActorRightVector() * -150.0f;

                FHitResult Hit;
                const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility, Params);

                bIsOnTheLeft = bBlocked;
            }

            if (bIsOnTheLeft && bIsOnTheRight)
            {
                TargetOffset = FVector::ZeroVector;

                ApplyCameraOffsets(*SpringArm, DeltaTime);

                bUsingSocketOffset = true;
            }
            else if (bIsOnTheRight)
            {
                const bool bSomeStatement = LastUsedOffset == 150.0f && bUsingSocketOffset;

                TargetOffset = bSomeStatement ? GetTargetOffset(150.0f) : GetTargetOffset(-150.0f);

                ApplyCameraOffsets(*SpringArm, DeltaTime);

                if (not bSomeStatement)
                {
                    bUsingSocketOffset = true;
                    LastUsedOffset = -150.0f;
                }
            }
            else if (bIsOnTheLeft)
            {
                const bool bSomeStatement = LastUsedOffset == -150.0f && bUsingSocketOffset;

                TargetOffset = bSomeStatement ? GetTargetOffset(-150.0f) : GetTargetOffset(150.0f);

                ApplyCameraOffsets(*SpringArm, DeltaTime);

                if (not bSomeStatement)
                {
                    bUsingSocketOffset = true;
                    LastUsedOffset = 150.0f;
                }
            }
            else
            {
                const float RightDot = UKismetMathLibrary::Dot_VectorVector(CameraOwner->GetActorForwardVector(), ViewTarget->GetActorRightVector());
                const float ForwardDot = UKismetMathLibrary::Dot_VectorVector(CameraOwner->GetActorForwardVector(), ViewTarget->GetActorForwardVector());

                const float DotCoefficient = fabs(RightDot) > fabs(ForwardDot) ? 1.0f : -1.0f;

                const float Result = RightDot * ForwardDot * DotCoefficient;

                const float OffsetCoefficient = Result < 0.0f ? -1.0f : 1.0f;

                LastUsedOffset = bUsingSocketOffset ? LastUsedOffset : OffsetCoefficient * fabs(LastUsedOffset);
                TargetOffset = GetTargetOffset(LastUsedOffset);

                ApplyCameraOffsets(*SpringArm, DeltaTime);

                bUsingSocketOffset = true;
            }
        }
    }
    else // Camera above ViewTarget
    {
        ApplyCameraOffsets(*SpringArm, DeltaTime);
        bUsingSocketOffset = false;
    }
}

FVector UDroneBoundCameraModifier::GetTargetOffset(float Offset) const
{
    const FVector CameraRight = CameraOwner->GetActorRightVector();

    const FVector RightOffset = CameraRight * Offset;

    return FMath::Lerp(FVector::ZeroVector, RightOffset, TargetOffsetLerpAlpha);
}

void UDroneBoundCameraModifier::ApplyCameraOffsets(USpringArmComponent& SpringArm, float DeltaTime) const
{
    if (TargetOffset == FVector::ZeroVector)
    {
        return;
    }

    // Any > 0 interp speed is too slow.
    const FVector NewOffset = UKismetMathLibrary::VInterpTo(SpringArm.TargetOffset, TargetOffset, DeltaTime, 0.0f);
    SpringArm.TargetOffset = NewOffset;
}
