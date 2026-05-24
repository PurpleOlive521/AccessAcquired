// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ReceiveDamageEventInterface.h"
#include "Misc/Optional.h"
#include "DamageCalculation.generated.h"

class UGameplaySystemComponent;

// Parameters for applying a DamageCalculation.
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FDamageCalculationParams
{
	GENERATED_BODY()

	FDamageCalculationParams() = default;

	float GetRandomDeviation() const;

	void ApplyOnEvent(FAADamageEvent& Event) const;

	bool HasValidDamageFalloff() const;

	// 100 is 100%.
	UPROPERTY(EditAnywhere, meta = (UIMin = 0, UIMax = 200, Units = "Percent"), Category = "Randomness")
	float RandomDeviationMin = 100.0f;

	// 100 is 100%.
	UPROPERTY(EditAnywhere, meta = (UIMin = 0, UIMax = 200, Units = "Percent"), Category = "Randomness")
	float RandomDeviationMax = 100.0f;

	// Applies the random deviation on the Damage value in the calculation.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Randomness")
	bool bApplyRandomOnDamage = true;

	// Applies the random deviation on the Charge value in the calculation.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Randomness")
	bool bApplyRandomOnCharge = false;

	// Use the same deviation for both Damage and Charge.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Randomness")
	bool bUseSameRandom = false;

	UPROPERTY(EditAnywhere)
	bool bHasDamageFalloff = false;

	// Expects a table representing the Damage Falloff curve as:
	//		X-axis - Distance as a 0 to 1 value with 0 representing the hitbox center and 1 representing the farthest possible distance away from it.
	//		Y-axis - Damage coefficient relative to Distance.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveFloat> DamageFalloffTable = nullptr;
};

// Per activation parameters
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FDamageCalculationLocals
{
	GENERATED_BODY()

	FDamageCalculationLocals() = default;

	FDamageCalculationLocals(float InDamageMultiplier, float InChargeMultiplier, float InKnockback);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DamageCalculationLocals")
	float DamageMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DamageCalculationLocals")
	float ChargeMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DamageCalculationLocals")
	float Knockback = 0.0f;

	static FDamageCalculationLocals Default;
};

/**
 * An damage algorithm implementation. Override the default implementation to create unique algorithms for different scenarios and events.
 * By default, damage algorithms generate DamageEvents in stages, where different kinds of data is processed before applying the event on a target.
 */
UCLASS(Blueprintable)
class VERTICALSLICE_API UDamageCalculation : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	static const UDamageCalculation* GetFromClass(TSubclassOf<UDamageCalculation> DamageCalculationClass);

	// Performs the entire pipeline of creating a DamageEvent and applying it on the Target. Requires all information that we can gather for the event.
	virtual TOptional<FAAResolvedDamageEvent> GenerateAndApplyDamageCalculation(AActor* Target, AActor* Instigator, UGameplaySystemComponent* InstigatorGS, 
																				UShapeComponent* InstigatorHitbox, const FHitResult& Hit, const FAADamageEventFlags& EventFlags, 
																				const FDamageCalculationLocals& Locals = FDamageCalculationLocals::Default) const;

	// TOptional is valid if we managed to call ReceiveDamageEvent on the Target. Does not guarantee that the target resolved the event, simply that they received it.
	virtual TOptional<FAAResolvedDamageEvent> ApplyDamageEvent(AActor* Target, FAADamageEvent DamageEvent) const;

	// Creates a DamageEvent from initial properties.
	virtual FAADamageEvent GenerateEvent(AActor* Instigator, 
										 UGameplaySystemComponent* InstigatorGS, 
										 const FAADamageEventFlags& EventFlags, 
										 const FDamageCalculationLocals& Locals = FDamageCalculationLocals::Default) const;

	// Applies an occured collision event on the DamageEvent.
	virtual void ApplyCollisionEvent(FAADamageEvent& DamageEvent, const FHitResult& Hit, UShapeComponent* InstigatorHitbox) const;

	UPROPERTY(EditAnywhere)
	FDamageCalculationParams Params;
};
