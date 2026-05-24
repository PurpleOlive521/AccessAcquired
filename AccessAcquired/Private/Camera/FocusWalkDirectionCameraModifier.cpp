// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "FocusWalkDirectionCameraModifier.h"
#include "GameplaySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayTagDefines.h"

bool UFocusWalkDirectionCameraModifier::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	const FVector CameraForward = CameraOwner->GetActorForwardVector();
	const FVector ViewTargetForward = ViewTarget->GetActorForwardVector();

	const float Dot2D = UKismetMathLibrary::DotProduct2D(FVector2D(CameraForward), FVector2D(ViewTargetForward));

	// Only apply the rotation if the difference in camera-to-viewtarget direction is large enough
	const bool bIsViewTargetFacingCamera = Dot2D < 0.0f;
	
	const float DotComparator = bIsViewTargetFacingCamera ? MinimumTowardsDot : MinimumAwayDot;
	if (fabs(Dot2D) > fabs(DotComparator))
	{
		return false;
	}

	// We scale the interpolator further by how far from the dot cutoff we are, so the rotation doesn't turn on as harshly
	const float Difference = (DotComparator - fabs(Dot2D)) / (1.0f - DotComparator);
	const float DotInterpolationScalar = FMath::Clamp(Difference, 0.0f, 1.0f);

	UGameplaySystemComponent* GameplaySystem = UGameplaySystemComponent::GetGameplaySystemFromActor(ViewTarget);
	if (not GameplaySystem)
	{
		return false;
	}
	
	const float GroundSpeed = ViewTarget->GetVelocity().Size2D();
	const float MovementSpeed = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MovementSpeed, EAttributeValue::EAV_CurrentValue);

	const float RelativeSpeed = FMath::Clamp( GroundSpeed / MovementSpeed, 0.0f, 1.0f);

	float InterpolationCoefficient = MaxInterpolation * DotInterpolationScalar;

	// Increase interpolation further if the ViewTarget is running
	if (GameplaySystem->HasTag(GAMEPLAYTAG_Status_Running))
	{
		if (bIncreaseInterpolationWhenRunning)
		{
			InterpolationCoefficient += AddedInterpolationWhenRunning;
		}
	}

	const float FinalInterpolation = RelativeSpeed * InterpolationCoefficient;
	if (FinalInterpolation < KINDA_SMALL_NUMBER)
	{
		return false;
	}

	const FRotator CurrentRotation = CameraOwner->GetCameraRotation();
	const FRotator DesiredRotation = UKismetMathLibrary::MakeRotFromX(ViewTargetForward);

	FRotator NewRotation = UKismetMathLibrary::RInterpTo(OutViewRotation, DesiredRotation, DeltaTime, FinalInterpolation);

	OutDeltaRot += FRotator(0.0, NewRotation.Yaw - OutViewRotation.Yaw, 0.0);

	return false;
}
