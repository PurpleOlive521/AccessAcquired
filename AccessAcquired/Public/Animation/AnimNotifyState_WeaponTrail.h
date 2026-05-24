// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/GSAnimNotifyState.h"
#include "AnimNotifyState_WeaponTrail.generated.h"

class UWeaponComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UAnimNotifyState_WeaponTrail : public UGSAnimNotifyState
{
	GENERATED_BODY()
	
public:

	UAnimNotifyState_WeaponTrail();

	// --- Begin UAnimNotifyState Interface
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	// --- End UAnimNotify Interface

protected:

	UPROPERTY(EditAnywhere)
	int32 WeaponIdentifier = 0;

	TWeakObjectPtr<UWeaponComponent> WeaponComponent = nullptr;
};
