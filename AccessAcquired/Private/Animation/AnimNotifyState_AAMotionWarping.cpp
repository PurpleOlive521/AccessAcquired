// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AnimNotifyState_AAMotionWarping.h"
#include "TargetLockingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RootMotionModifier_SkewWarp.h"
#include "DevCommons.h"

UAnimNotifyState_AAMotionWarping::UAnimNotifyState_AAMotionWarping(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif //WITH_EDITORONLY_DATA
}

void UAnimNotifyState_AAMotionWarping::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (bAllowPhysicsRotationDuringNotify)
		{
			if (UCharacterMovementComponent* CharacterMovementPtr = Owner->GetComponentByClass<UCharacterMovementComponent>())
			{
				CharacterMovementPtr->bAllowPhysicsRotationDuringAnimRootMotion = true;
				CharacterMovement = MakeWeakObjectPtr(CharacterMovementPtr);
			}
		}

		if (UTargetLockingComponent* TargetLockingPtr = Owner->GetComponentByClass<UTargetLockingComponent>())
		{
			if (not bUseExternalWarpTarget)
			{
				TargetLockingPtr->TriggerMotionWarp(Query);
				TargetLocking = MakeWeakObjectPtr(TargetLockingPtr);
			}
		}
	}
}

void UAnimNotifyState_AAMotionWarping::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_AAMotionWarping::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (bAllowPhysicsRotationDuringNotify)
	{
		if (UCharacterMovementComponent* CharacterMovementPtr = CharacterMovement.Get())
		{
			CharacterMovementPtr->bAllowPhysicsRotationDuringAnimRootMotion = false;
		}
	}

	if (bRemoveWarpTargetOnNotifyEnd)
	{
		if (UTargetLockingComponent* TargetLockingPtr = TargetLocking.Get())
		{
			if (bUseExternalWarpTarget)
			{
				// Remove the external target based on the expected name
				if (URootMotionModifier_SkewWarp* Modifier = Cast<URootMotionModifier_SkewWarp>(RootMotionModifier))
				{
					TargetLockingPtr->RemoveWarpTarget(Modifier->WarpTargetName);
				}
				else
				{
					UE_LOG(VSLog, Error, TEXT("AnimNotifyState_AAMotionWarping: Could not remove external WarpTarget. RootMotionModifier was not of type SkewWarp."));
				}
			}
			else
			{
				TargetLockingPtr->RemoveComponentWarpTarget();
			}
		}
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}