// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AAProjectileMovementComponent.h"
#include "DamageEventTypes.h"
#include "ProjectileBase.generated.h"

class USphereComponent;
class UDamageCalculation;

UCLASS()
class VERTICALSLICE_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectileBase(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "ProjectileBase")
	void InitializeProjectile(AActor* InInstigator, const FRotator& Direction);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Initialize Projectile"), Category = "ProjectileBase")
	void K2_InitializeProjectile();

	UFUNCTION()
	void OnHitObject(const FHitResult& Hit);

	void OnExpired();

	void OnExplode();

	// This Actor must be destroyed as part of this process!
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "On Explode"), Category = "ProjectileBase")
	void K2_OnExplode();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ProjectileBase")
	float MaxLifetime = 10.0f;

	// Damage multiplier that increases or decreases the damage this deals.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ProjectileBase")
	float RelativeStrength = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ProjectileBase")
	float Knockback = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ProjectileBase")
	FAADamageEventFlags EventFlags;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ProjectileBase")
	TSubclassOf<UDamageCalculation> DamageCalculationClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectileBase")
	TObjectPtr<UAAProjectileMovementComponent> ProjectileMovement = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ProjectileBase")
	TObjectPtr<USphereComponent> Collider = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "ProjectileBase")
	FAADamageEvent PendingEvent;
	ETeam Team = ETeam::ET_NotAssigned;

	FTimerHandle LifetimeHandle;
};
