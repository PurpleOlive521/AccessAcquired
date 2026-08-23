// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Components/AASpringArmComponent.h"

UAASpringArmComponent::UAASpringArmComponent(
    const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

void UAASpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
    TargetArmLength = BaseTargetArmLength + TargetArmLengthModifier;

    SocketOffset = BaseSocketOffset + SocketOffsetModifier;

    Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);

    TargetArmLengthModifier = 0.0f;
    SocketOffsetModifier = FVector::ZeroVector;
}