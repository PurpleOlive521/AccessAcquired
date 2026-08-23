// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "ProjectileBase.h"
#include "Components/CapsuleComponent.h"
#include "DamageCalculation.h"
#include "GameplaySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayUtilityBlueprintLibrary.h"

AProjectileBase::AProjectileBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	CustomRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Custom Root"));
	CustomRoot->SetMobility(EComponentMobility::Movable);
	SetRootComponent(CustomRoot);

	Collider = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CapsuleCollider"));
	Collider->SetMobility(EComponentMobility::Movable);
	Collider->SetupAttachment(CustomRoot);
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


	TArray<FHitResult> HitResults;
	Collider->OnComponentBeginOverlap.AddUniqueDynamic(this, &AProjectileBase::OnHitObject);

	FComponentQueryParams QueryParams = {};
	const FVector SweepLocation = Collider->GetComponentLocation();
	GetWorld()->ComponentSweepMulti(HitResults, Collider, SweepLocation, SweepLocation, Collider->GetComponentRotation(), QueryParams);

	for (const auto& Hit : HitResults)
	{
		OnHitObject(Collider, Hit.GetActor(), Hit.GetComponent(), 0, true, Hit);
	}

	K2_InitializeProjectile();
}

void AProjectileBase::OnHitObject(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->Implements<UReceiveDamageEventInterface>())
	{
		if (!OtherActor->Implements<UReceiveDamageEventInterface>())
		{
			return;
		}

		if (IReceiveDamageEventInterface::Execute_IsDead(OtherActor))
		{
			return;
		}

		const bool bSameTeam = UGameplayUtilityBlueprintLibrary::AreSameTeam_OneEnum(OtherActor, Team);
		if (bSameTeam)
		{
			return;
		}

		if (const UDamageCalculation* DamageCalculation = UDamageCalculation::GetFromClass(DamageCalculationClass))
		{
			DamageCalculation->ApplyCollisionEvent(PendingEvent, SweepResult, Collider);
			DamageCalculation->ApplyDamageEvent(OtherActor, PendingEvent);
		}
	}
}



