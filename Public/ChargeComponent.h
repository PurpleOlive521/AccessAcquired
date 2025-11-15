// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GameplayTagContainer.h"
#include "DamageEventTypes.h"

#include "ChargeComponent.generated.h"

class UGameplaySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOverchargedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecoveredSignature);

//State machine responsible for keeping track of broken time, recovery time, and current state
UENUM(BlueprintType)
enum class EChargeState : uint8
{
	ECS_Normal			UMETA(DisplayName = "Normal"),
	ECS_Overcharged		UMETA(DisplayName = "Overcharged"),
	ECS_Recovering		UMETA(DisplayName = "Recovering")
};

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VERTICALSLICE_API UChargeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UChargeComponent();

	// --- Begin UActorComponent Interface
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// --- End UActorComponent Interface

	UFUNCTION(BlueprintCallable)
	FAAResolvedDamageEvent ResolveDamageEvent(const FAADamageEvent& DamageEvent);

protected:

	void NormalState(float DeltaTime);

	// Called when switched to Normal state.
	void OnEnterNormalState();

	void RecoveringState(float DeltaTime);

	// Called when switched to Recovering state.
	void OnEnterRecoveringState();

	void OverchargedState(float DeltaTime);

	// Called when switched to OverCharged state.
	void OnEnterOverchargedState();


	UFUNCTION(BlueprintCallable)
	void SetChargeState(EChargeState InState);

	UPROPERTY(BlueprintReadOnly)
	EChargeState ChargeState = EChargeState::ECS_Normal;

	UPROPERTY(BlueprintReadOnly)
	float BrokenTimeCounter = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float RecoveryTimeCounter = 0.0f;

private:
	TObjectPtr<UGameplaySystemComponent> GameplaySystem;

public:	
	// --- Delegates
	UPROPERTY(BlueprintAssignable)
	FOnOverchargedSignature OnOverchargedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnRecoveredSignature OnRecoveredDelegate;
};
