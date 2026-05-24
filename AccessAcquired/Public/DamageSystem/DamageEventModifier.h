// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DamageEventTypes.h"
#include "HitboxAsset.h"
#include "GameplayEffect.h"
#include "DamageEventModifier.generated.h"

class UHitboxSourceComponent;

/*
* The plan here is as follows:
* DamageEventModifiers are instantiated upon addition to a HitboxSourceComponent. There is a always-used default one (possibly tsubclassof exposed)
* where any generic modifiers are applied.
* 
* The DamageEventModifiers are then applied to any outgoing damageEvent based on the API as follows:
* 1. CanApply check which by default checks against Hitbox type GameplayTag
* 2. If CanApply passes, apply modifiers onto the outgoing DamageEvent. In the case of Ailments, applies the correct ailment based on the outgoing DamageType
* 
* DamageEventModifiers are stateful if needed and should have a Blueprint interface too, which is the same functions but K2_ marked to make it easier.
*/

/**
 * 
 */
UCLASS(Blueprintable)
class VERTICALSLICE_API UDamageEventModifier : public UObject
{
	GENERATED_BODY()

public:

	UDamageEventModifier();

	void AddModifier(UHitboxSourceComponent& OwningComponent);
	
	void RemoveModifier();

	// Main entry call to applying a DamageEventModifiers.
	// Checks if the modifier is appliable first before attempting to apply the modifier.
	bool TryApplyDamageEventModifier(FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams);

	bool CheckCanApplyModifier(const FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams) const;

	void ApplyDamageEventModifier(FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams);

	UFUNCTION(BlueprintCallable, Category = "DamageEventModifier")
	UGameplaySystemComponent* GetGameplaySystem() const;

	UFUNCTION(BlueprintCallable, Category = "DamageEventModifier")
	AActor* GetInstigator() const;
	
	// Disabled DamageEventModifiers are never applied.
	UFUNCTION(BlueprintCallable, Category = "DamageEventModifier")
	void SetIsEnabled(bool bInState);

	// Called when the DamageEventModifier is added to a HitboxSourceComponent.
	virtual void OnModifierAdded();

	// Called when the DamageEventModifier is added to a HitboxSourceComponent.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Blueprint On Modifier Added"), Category = "DamageEventModifier")
	void K2_OnModifierAdded();

	// Called when the DamageEventModifier is removed from a HitboxSourceComponent.
	virtual void OnModifierRemoved();

	// Called when the DamageEventModifier is removed from a HitboxSourceComponent.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Blueprint On Modifier Removed"), Category = "DamageEventModifier")
	void K2_OnModifierRemoved();

	// Applies the DamageEventModifier on a outgoing DamageEvent.
	virtual void ApplyModifier(FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams);

	// Applies the DamageEventModifier on a outgoing DamageEvent.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Blueprint Apply Modifier"), Category = "DamageEventModifier")
	void K2_ApplyModifier(UPARAM(ref) FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams);
	
	// Returns true if the DamageEventModifier can be applied to the DamageEvent, false otherwise.
	virtual bool CanApplyModifier(const FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams) const;

	// Returns true if the DamageEventModifier can be applied to the DamageEvent, false otherwise.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Blueprint Can Apply Modifier"), Category = "DamageEventModifier")
	bool K2_CanApplyModifier(const FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams) const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DamageEventModifier|Modifiers")
	TArray<TSubclassOf<UGameplayEffect>> EffectsOnSelf;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DamageEventModifier|Modifiers")
	TArray<TSubclassOf<UGameplayEffect>> EffectsOnTarget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DamageEventModifier|Modifiers")
	TArray<FAssociatedGameplayEffectStackModifier> StackModifiersOnTarget;

	// Applied before all other modifiers
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DamageEventModifier|Modifiers")
	float DamageScalar = 1.0f;

	// Applied before all other modifiers
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DamageEventModifier|Modifiers")
	float ChargeScalar = 1.0f;

	// Applied before all other modifiers
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DamageEventModifier|Modifiers")
	float KnockbackScalar = 1.0f;

	// Which Hitbox types this by default applies to.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition  = "bApplyToAnyHitboxType == false"), Category = "DamageEventModifier")
	FGameplayTagContainer AllowedHitboxTypes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DamageEventModifier")
	bool bApplyToAnyHitboxType = true;

protected:

	UPROPERTY(BlueprintReadWrite, Category = "DamageEventModifier")
	TObjectPtr<UHitboxSourceComponent> HitboxSourceComponent = nullptr;

	TWeakObjectPtr<UGameplaySystemComponent> GameplaySystem = nullptr;

	bool bIsEnabled = true;

	bool bHasBlueprintCanApplyModifier = false;
};
