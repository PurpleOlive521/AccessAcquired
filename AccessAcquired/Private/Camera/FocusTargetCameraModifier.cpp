// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "FocusTargetCameraModifier.h"
#include "Kismet/KismetMathLibrary.h"
#include "TargetLockingComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

bool UFocusTargetCameraModifier::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	AActor* Target = OverrideTarget;

	if (not Target)
	{
		if (not ViewTarget)
		{
			return false;
		}

		// TODO: Either make an interface or quicker getter for it, or atleast make it a static function so we always get TargetLockingComponents the same way.
		UTargetLockingComponent* TargetLockingComp = ViewTarget->GetComponentByClass<UTargetLockingComponent>();
		if (not TargetLockingComp)
		{
			return false;
		}

		Target = TargetLockingComp->GetTargetedActor();
		if (not Target)
		{
			return false;
		}		
	}

	const FVector CameraLocation = CameraOwner->GetCameraLocation();
	FVector TargetLocation = Target->GetActorLocation();

	if (Target->IsA<ACharacter>())
	{
		ACharacter* TargetAsCharacter = Cast<ACharacter>(Target);
		
		if (UCapsuleComponent* Capsule = TargetAsCharacter->GetCapsuleComponent())
		{
			TargetLocation.Z += Capsule->GetScaledCapsuleHalfHeight();
		}
	}

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TargetLocation);

	LookAtRotation.Pitch = FMath::Clamp(LookAtRotation.Pitch, -MinimumAllowedPitch, MinimumAllowedPitch);

	FRotator DesiredRotation = FMath::RInterpTo(OutViewRotation, LookAtRotation, DeltaTime, SlewSpeed);

	OutDeltaRot += DesiredRotation - OutViewRotation;

	return true;
}

