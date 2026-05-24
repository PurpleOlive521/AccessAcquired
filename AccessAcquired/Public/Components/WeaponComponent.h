// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "ReceiveDamageEventInterface.h"
#include "DamageCalculation.h"
#include "GameplayTagContainer.h"
#include "WeaponAttachmentAsset.h"
#include "WeaponComponent.generated.h"

class UWeaponAnimInstance;

// Will always be matchable for GetActiveSkeletalMesh. Make sure not to use in actual game code!
const FName AnimationPreviewerMeshTag = TEXT("AnimationPreviewerMeshTag");

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponHitSignature);

/**
 * A weapon component that can trace for objects and apply damage calculations to them.
 * Derivable to allow customisation in the trace algorithm per weapon.
 */
UCLASS(HideDropdown)
class VERTICALSLICE_API UWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:

	UWeaponComponent(const FObjectInitializer& ObjectInitializer);

	virtual void OnRegister() override;

	virtual void BeginPlay() override;

#if WITH_EDITORONLY_DATA
	virtual void PreEditChange(FProperty* PropertyAboutToChange);

	virtual void PreEditChange(class FEditPropertyChain& PropertyAboutToChange);

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITORONLY_DATA

	// Tries through the MeshAccessorInterface first, resorting to a component search if it returns nullptr.
	UFUNCTION(BlueprintCallable)
	static UWeaponComponent* GetWeaponFromActor(AActor* Actor, int32 Identifier = 0);

	UFUNCTION()
	void OnHit(const FAAResolvedDamageEvent& ResolvedEvent);
	
	// Called on a successful hit, that passes all checks and is not ignored by the target.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Hit"))
	void K2_OnHit(const FAAResolvedDamageEvent& ResolvedEvent);

	// Returns the SkeletalMeshComponent we are acting upon. Is either self or OverrideSkeletalMesh.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	USkeletalMeshComponent* GetActiveSkeletalMesh();

	// Enables weapon trails if not already enabled. Will fail silently if the no trails are used for the weapon.
	UFUNCTION(BlueprintCallable)
	void EnableWeaponTrails();

	// Disables weapon trails if already enabled.
	UFUNCTION(BlueprintCallable)
	void DisableWeaponTrails();

	// Triggers the transition in the weapons AnimInstance.
	UFUNCTION(BlueprintCallable)
	void PlayEquipAnimation();

	UFUNCTION(BlueprintCallable)
	void PlayUnequipAnimation();

	void PerformAutomaticAttachment();

	void UndoAutomaticAttachment();

	void SetIdentifier(int32 InIdentifier);

	int32 GetIdentifier() const;

protected:

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Enable Weapon Trails"))
	void K2_EnableWeaponTrails();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Disable Weapon Trails"))
	void K2_DisableWeaponTrails();

	// Can return nullptr.
	inline UWeaponAnimInstance* GetWeaponAnimInstance();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon")
	TObjectPtr<UWeaponAttachmentAsset> AttachmentRules = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon")
	bool bDisableAutomaticAttachment = false;

	// Unique identifier to differentiate between weapons. Ensure it's unique on the owning Actor!
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon")
	int32 Identifier = 0;

	// Configures the component to be used as a stand-in, where we perform logic on the OverrideSkeletalMesh instead of ourselves
	// Useful in cases when we can't have the Weapon be implemented as a WeaponComponent directly.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon")
	bool bUseOverrideSkeletalMesh = false;

	// The tag we query for to find a OverrideSkeletalMesh.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = bUseOverrideSkeletalMesh), Category = "Weapon")
	FName OverrideSkeletalMeshTag;

	TObjectPtr<USkeletalMeshComponent> OverrideSkeletalMesh = nullptr;

private:

	bool bHasEnabledTrails = false;

	bool bHasEquippedWeapon = false;

public:
	// Called once per hit, after all calculations are done.
	UPROPERTY(BlueprintAssignable)
	FOnWeaponHitSignature OnWeaponHitDelegate;
};
