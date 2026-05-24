// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "DamageCalculation.h"
#include "GameplaySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"

AProjectileBase::AProjectileBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	Collider = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Collider"));
	SetRootComponent(Collider);

	ProjectileMovement = ObjectInitializer.CreateDefaultSubobject<UAAProjectileMovementComponent>(this, TEXT("ProjectileMovement"));

	ProjectileMovement->OnProjectileStop.AddUniqueDynamic(this, &AProjectileBase::OnHitObject);
	ProjectileMovement->Velocity =		FVector{ -1.0f, 0.0f, 0.0f };
	ProjectileMovement->InitialSpeed =	2500.0f;
	ProjectileMovement->MaxSpeed =		2500.0f;
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileBase::InitializeProjectile(AActor* InInstigator, const FRotator& Direction)
{
	if (InInstigator->Implements<UReceiveDamageEventInterface>())
	{
		Team = IReceiveDamageEventInterface::Execute_GetTeam(InInstigator);

		UGameplaySystemComponent* GameplaySystem = UGameplaySystemComponent::GetGameplaySystemFromActor(InInstigator);

		ensure(GameplaySystem);

		if (const UDamageCalculation* DamageCalculation = UDamageCalculation::GetFromClass(DamageCalculationClass))
		{
			FDamageCalculationLocals Locals = { RelativeStrength, RelativeStrength, Knockback };
			PendingEvent = DamageCalculation->GenerateEvent(InInstigator, GameplaySystem, EventFlags, Locals);
		}
	}

	ProjectileMovement->SetInstigator(InInstigator);

	ProjectileMovement->Velocity = Direction.Vector() * ProjectileMovement->InitialSpeed;

	GetWorldTimerManager().SetTimer(LifetimeHandle, this, &AProjectileBase::OnExpired, MaxLifetime);

	K2_InitializeProjectile();
}

void AProjectileBase::OnHitObject(const FHitResult& Hit)
{
	if (AActor* Target = Hit.GetActor())
	{
		if (const UDamageCalculation* DamageCalculation = UDamageCalculation::GetFromClass(DamageCalculationClass))
		{
			DamageCalculation->ApplyCollisionEvent(PendingEvent, Hit, Collider);
			DamageCalculation->ApplyDamageEvent(Target, PendingEvent);
		}
	}

	OnExplode();
}

void AProjectileBase::OnExpired()
{
	OnExplode();
}

void AProjectileBase::OnExplode()
{
	Collider->SetGenerateOverlapEvents(false);

	GetWorldTimerManager().ClearTimer(LifetimeHandle);

	K2_OnExplode();
}



