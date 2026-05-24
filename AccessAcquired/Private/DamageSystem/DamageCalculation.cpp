// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "DamageCalculation.h"
#include "GameplaySystemComponent.h"
#include "ModeComponent.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "DevelopmentTypes.h"

float FDamageCalculationParams::GetRandomDeviation() const
{
	return FMath::FRandRange(RandomDeviationMin * 0.01, RandomDeviationMax * 0.01);
}

void FDamageCalculationParams::ApplyOnEvent(FAADamageEvent& Event) const
{
	float Deviation = GetRandomDeviation();

	if (bApplyRandomOnDamage)
	{
		Event.Damage *= Deviation;
	}

	if (!bUseSameRandom)
	{
		Deviation = GetRandomDeviation();
	}

	if (bApplyRandomOnCharge)
	{
		Event.AppliedCharge *= Deviation;
	}
}

bool FDamageCalculationParams::HasValidDamageFalloff() const
{
	if (bHasDamageFalloff)
	{
		if (DamageFalloffTable)
		{
			return true;
		}
	}

	return false;
}

FDamageCalculationLocals::FDamageCalculationLocals(float InDamageMultiplier, float InChargeMultiplier, float InKnockback)
	: DamageMultiplier(InDamageMultiplier), ChargeMultiplier(InChargeMultiplier), Knockback(InKnockback)
{

};

FDamageCalculationLocals FDamageCalculationLocals::Default = FDamageCalculationLocals(1.0f /* DmgMlt */, 1.0f /* ChgMlt */, 0.0f /* Knb*/);

const UDamageCalculation* UDamageCalculation::GetFromClass(TSubclassOf<UDamageCalculation> DamageCalculationClass)
{
	UObject* CDO = UDamageCalculation::StaticClass()->GetDefaultObject();

	if (DamageCalculationClass)
	{
		CDO = DamageCalculationClass->GetDefaultObject();
	}

	return Cast<UDamageCalculation>(CDO);
}

TOptional<FAAResolvedDamageEvent> UDamageCalculation::GenerateAndApplyDamageCalculation(AActor* Target, AActor* Instigator, UGameplaySystemComponent* InstigatorGS, UShapeComponent* InstigatorHitbox, const FHitResult& Hit, const FAADamageEventFlags& EventFlags, const FDamageCalculationLocals& Locals) const
{
	FAADamageEvent DamageEvent = GenerateEvent(Instigator, InstigatorGS, EventFlags, Locals);

	ApplyCollisionEvent(DamageEvent, Hit, InstigatorHitbox);

	return ApplyDamageEvent(Target, DamageEvent);
}

// Note: FHitResult does not hold any detailed collision data (normals, collisionpoint, etc.)
TOptional<FAAResolvedDamageEvent> UDamageCalculation::ApplyDamageEvent(AActor* Target, FAADamageEvent DamageEvent) const
{
	Params.ApplyOnEvent(DamageEvent);

	FAAResolvedDamageEvent ResolvedEvent = IReceiveDamageEventInterface::Execute_ReceiveDamageEvent(Target, DamageEvent);

	return TOptional<FAAResolvedDamageEvent>(ResolvedEvent);
}

FAADamageEvent UDamageCalculation::GenerateEvent(AActor* Instigator, UGameplaySystemComponent* InstigatorGS, const FAADamageEventFlags& EventFlags, const FDamageCalculationLocals& Locals) const
{
	EDamageType DamageType = EDamageType::EDT_Normal;

	UModeComponent* ModeComponent = Instigator->GetComponentByClass<UModeComponent>();
	if (ModeComponent)
	{
		DamageType = ModeComponent->GetMode();
	}

	FAADamageEvent DamageEvent;
	DamageEvent.Damage = Locals.DamageMultiplier * InstigatorGS->GetAttributeValue(EAttributeType::EAT_Damage, EAttributeValue::EAV_CurrentValue);
	DamageEvent.AppliedCharge = Locals.ChargeMultiplier * InstigatorGS->GetAttributeValue(EAttributeType::EAT_AppliedCharge, EAttributeValue::EAV_CurrentValue);
	DamageEvent.Knockback = Locals.Knockback;
	DamageEvent.Level = InstigatorGS->GetEntityLevel();
	DamageEvent.DamageType = DamageType;
	DamageEvent.DamageSource = Instigator;
	DamageEvent.StaggerDamage = InstigatorGS->GetAttributeValue(EAttributeType::EAT_StaggerDamage, EAttributeValue::EAV_CurrentValue);
	DamageEvent.EventFlags = EventFlags;

	return DamageEvent;
}

void UDamageCalculation::ApplyCollisionEvent(FAADamageEvent& DamageEvent, const FHitResult& Hit, UShapeComponent* InstigatorHitbox) const
{
	DamageEvent.HitLocation = Hit.ImpactPoint;

	// Calculate and apply falloff
	if (Params.HasValidDamageFalloff())
	{
		float DistanceScalar = 1.0f;

		if (Hit.bStartPenetrating)
		{
			if (UPrimitiveComponent* Primitive = Hit.Component.Get())
			{
				const float DistanceFromHitbox = FVector::Distance(InstigatorHitbox->GetComponentLocation(), Hit.ImpactPoint);
				float MaxPossibleDistance = Hit.Distance + Hit.PenetrationDepth; // Approximation

				if (USphereComponent* SphereCollider = Cast<USphereComponent>(InstigatorHitbox))
				{
					MaxPossibleDistance = SphereCollider->GetScaledSphereRadius();
				}
				else
				{
					ensureNoEntry(); // Only use Falloffs on spherical colliders, and not capsules! 
				}

				DistanceScalar = DistanceFromHitbox / MaxPossibleDistance;
				DistanceScalar = FMath::Clamp(DistanceScalar, 0.0f, 1.0f);
			}

		}

		check(Params.DamageFalloffTable);

		const float DamageCoefficient = Params.DamageFalloffTable->GetFloatValue(DistanceScalar);

		DamageEvent.Damage *= DamageCoefficient;
		DamageEvent.AppliedCharge *= DamageCoefficient;
	}
}