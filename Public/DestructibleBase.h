// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SaveableObjectInterface.h"
#include "GuidTag.h"
#include "ReceiveDamageEventInterface.h"

#include "DestructibleBase.generated.h"

UCLASS(Blueprintable)
class VERTICALSLICE_API ADestructibleBase : public AActor, public ISaveableObjectInterface, public IReceiveDamageEventInterface
{
	GENERATED_BODY()
	
public:	
	ADestructibleBase();

	// --- Begin AActor Interface
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// --- End AActor Interface

	// Ensures that the ID is not copied over when the Actor is duplicated in the Editor.
	virtual void PostEditImport() override;

	// --- Begin ISaveableObjectInterface
	virtual void SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject) override;

	virtual void LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject) override;
	// --- End ISaveableObjectInterface

	// --- Begin IReceiveDamageEventInterface
	virtual FAAResolvedDamageEvent ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent) override;

	virtual bool IsDead_Implementation() override;

	virtual ETeam GetTeam_Implementation() override;
	// --- End IReceiveDamageEventInterface


	virtual void OnDestructed(bool bIsOnLoad);

	// Blueprints opportunity to act when the destructible is destroyed.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Destructed"))
	void K2_OnDestructed(bool bIsOnLoad);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Damage Taken"))
	void K2_OnDamageTaken();

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Health = 50.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GainedExperience = 25.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GainedIntel = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsDestroyed = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGuidTag Id;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ETeam Team = ETeam::ET_Enemy;
};
