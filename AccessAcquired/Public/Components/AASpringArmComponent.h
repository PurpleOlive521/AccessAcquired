// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "AASpringArmComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class VERTICALSLICE_API UAASpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
    UAASpringArmComponent(const FObjectInitializer& ObjectInitializer);

    // Natural length of the spring arm when there are no collisions.
    // Preferred when setting component defaults as TargetArmLength is volatile when using camera modifiers.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AACamera")
    float BaseTargetArmLength = 0.0f;

    // Value to add to the target arm length, e.g. caused by camera modifiers.
    float TargetArmLengthModifier = 0.0f;

    // Offset at end of spring arm; use this instead of the relative offset of the attached component to ensure the line trace works as desired
    // Preferred when setting component defaults as SocketOffset is volatile when using camera modifiers.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AACamera")
    FVector BaseSocketOffset = FVector::ZeroVector;

    // Value to add to the socket offset, e.g. caused by camera modifiers.
    FVector SocketOffsetModifier = FVector::ZeroVector;

protected:

    virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
};
