// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyState_MotionWarping.h"
#include "TargetLockingComponent.h"
#include "AnimNotifyState_AAMotionWarping.generated.h"

class UCharacterMovementComponent;
class UTargetLockingComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UAnimNotifyState_AAMotionWarping : public UAnimNotifyState_MotionWarping
{
	GENERATED_BODY()

public:

	UAnimNotifyState_AAMotionWarping(const FObjectInitializer& ObjectInitializer);

	// --- Begin UAnimNotifyState Interface
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	// --- End UAnimNotifyState Interface

protected:

	// The TargetLockingComponent is not responsible for creating the WarpTarget.
	// Expects external code to create one during the MotionWarping window.
	UPROPERTY(EditAnywhere, Category = "Motion Warping")
	bool bUseExternalWarpTarget = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseExternalWarpTarget == false"), Category = "Motion Warping")
	FMotionWarpQuery Query;

	// Allows external rotation to affect the animation. 
	UPROPERTY(EditAnywhere, Category = "Motion Warping")
	bool bAllowPhysicsRotationDuringNotify = true;

	UPROPERTY(EditAnywhere, Category = "Motion Warping")
	bool bRemoveWarpTargetOnNotifyEnd = true;

	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovement = nullptr;

	TWeakObjectPtr<UTargetLockingComponent> TargetLocking = nullptr;
};
