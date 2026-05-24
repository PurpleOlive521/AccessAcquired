// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GateControllerComponent.h"
#include "GateComponent.generated.h"

class ULatentCurveEvaluator;

/**
 * A Static Mesh that is driven by a CurveFloat to animate a opening and closing motion.
 */
UCLASS()
class VERTICALSLICE_API UGateComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UGateComponent(const FObjectInitializer& ObjectInitializer);

	// --- Begin UActorComponent Interface
	virtual void OnRegister() override;

	virtual void PostEditImport() override;

	// Updates state and plays animations
	void SetGateState(EGateState InState);

	// Updates state without playing animations
	void SetGateState_Instantaneous(EGateState InState);

	// Updates internal state without affecting meshes
	void SetGateState_Internal(EGateState InState);

	UFUNCTION()
	void OnAnimationUpdated(float EvaluatedFloat);

	UFUNCTION()
	void OnAnimationFinished();

protected:
	// The distance this component will move during it's animation.
	UPROPERTY(EditDefaultsOnly, Category = "GateComponent")
	float TravelDistance = 260.0f;

	// The curve we source movement from.
	UPROPERTY(EditDefaultsOnly, Category = "GateComponent")
	UCurveFloat* AnimationCurve = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ULatentCurveEvaluator> AnimationEvaluator = nullptr;

	// The direction that the component moves in, relative to it's parent.
	UPROPERTY(EditDefaultsOnly, Category = "GateComponent")
	FVector MovingDirection = {};

	UPROPERTY(EditDefaultsOnly, Category = "GateComponent")
	FVector StartPosition = {};

	EGateState State = EGateState::EGS_Closed;
};
