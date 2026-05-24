// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SaveableObjectInterface.h"
#include "ReceiveDamageEventInterface.h"

#include "DestructibleBase.generated.h"

UCLASS(Blueprintable)
class VERTICALSLICE_API ADestructibleBase : public AActor, public ISaveableObjectInterface, public IReceiveDamageEventInterface
{
	GENERATED_BODY()
	
public:	
	ADestructibleBase();

	// --- Begin IReceiveDamageEventInterface
	virtual FAAResolvedDamageEvent ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent) override;

	virtual bool IsDead_Implementation() override;

	virtual ETeam GetTeam_Implementation() override;
	// --- End IReceiveDamageEventInterface

	virtual void OnDestructed(AActor* Instigator);

	// Blueprints opportunity to act when the destructible is destroyed.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Destructed"))
	void K2_OnDestructed();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Damage Taken"))
	void K2_OnDamageTaken();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GainedExperience = 25.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GainedIntel = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	float Health = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, SaveGame)
	bool bIsDestroyed = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETeam Team = ETeam::ET_Enemy;
};
