// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AAProjectileMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UAAProjectileMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Game|Components|ProjectileMovement")
	void SetInstigator(AActor* InInstigator);

protected:

	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0.0f, const FVector& MoveDelta = FVector::ZeroVector) override;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	bool bIgnoreCollisionsWithInstigator = true;

	TWeakObjectPtr<AActor> Instigator = nullptr;
};
