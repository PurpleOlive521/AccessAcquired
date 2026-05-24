// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/GSAnimNotifyState.h"
#include "HitboxSourceComponent.h"
#include "AnimNotifyState_SpawnHitbox.generated.h"

class UWeaponComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UAnimNotifyState_SpawnHitbox : public UGSAnimNotifyState
{
	GENERATED_BODY()

public:

	UAnimNotifyState_SpawnHitbox();

	// --- Begin UAnimNotifyState Interface
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void ResetDisplayWeapon();

	// --- End UAnimNotifyState Interface

	UFUNCTION()
	TArray<FName> GetSocketNames() const;

	UPROPERTY(EditAnywhere, Category = "SpawnHitbox")
	FHitboxParams Params;

protected:

#if WITH_EDITORONLY_DATA

	// The weapon we use as a placeholder for showcasing in the Animation Previewer.
	UPROPERTY(EditAnywhere, Category = "SpawnHitbox|Editor")
	TSoftClassPtr<UWeaponComponent> DisplayWeapon;

	TWeakObjectPtr<USkeletalMeshComponent> SkeletalMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "SpawnHitbox")
	bool bResetDisplayWeapon = false;

#endif //WITH_EDITORONLY_DATA

	TWeakObjectPtr<UHitboxSourceComponent> HitboxSourceComponent = nullptr;

	TWeakObjectPtr<UWeaponComponent> WeaponComponent;

	FSpawnedHitboxHandle Handle;
};
