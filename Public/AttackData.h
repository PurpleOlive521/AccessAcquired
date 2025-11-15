// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayEffect.h"

#include "AttackData.generated.h"

class UForceFeedbackEffect;

UCLASS(Blueprintable)
class VERTICALSLICE_API UAttackData : public UObject
{
	GENERATED_BODY()

public:
	UAttackData() ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Name = "Attack Name";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* AnimationMontage;

	// 2.0f is double the damage, 0.5f is half the damage etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Units="Times"))
	float DamageMultiplier = 1.0f;

	// If this value is above the Stagger Threshold, stagger the entity (hit animation)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StaggerDamage = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffectsOnTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffectsOnSelf;

	// Higher value for more impactful attacks, 1 is the default and expected for most attacks. A value of 0 circumvents the system entirely
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemySpecificData")
	int AttackTokenCost = 1;

	// The maximum distance from a Target that the attack can be performed in
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemySpecificData")
	float AttackRange = 150.0f;

	// Attack can't be performed within this distance of a target
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemySpecificData")
	float MinimumAttackRange = 0.0f;


};
