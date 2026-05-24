// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveableObjectInterface.h"
#include "GateControllerComponent.generated.h"

class UGateComponent;

UENUM(BlueprintType)
enum class EGateState : uint8
{
	EGS_Open		UMETA(DisplayName = "Open"),
	EGS_Closed		UMETA(DisplayName = "Closed"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGateStatesChangedSignature, EGateState, NewState);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VERTICALSLICE_API UGateControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGateControllerComponent();

	// --- Begin UActorComponent Interface
	virtual void OnRegister() override;

	virtual void InitializeComponent() override;
	// --- End UActorComponent Interface

	bool OnSerialize(FSaveGameArchive& Archive, bool bIsLoading);

	UFUNCTION(BlueprintCallable)
	void OpenGates();

	UFUNCTION(BlueprintCallable)
	void CloseGates();

	UFUNCTION(BlueprintCallable)
	void InvertGatesState();

	// Updates state and prompts animations on affectd Gates.
	void SetGateStates(EGateState InState);

	// Updates state without prompting animations on affected Gates.
	void SetGateStates_Instantaneous(EGateState InState);

protected:
	// Updates the state of all contained Gates to match this controllers state.
	inline void SyncGates();

	void GatherGateComponents();

	inline void SetLeaderState(EGateState InState);

	UPROPERTY(SaveGame)
	EGateState LeaderState = EGateState::EGS_Closed;

	TArray<TWeakObjectPtr<UGateComponent>> GateComponents;

public:
	// --- Delegates
	UPROPERTY(BlueprintAssignable)
	FOnGateStatesChangedSignature OnGateStatesChangedDelegate;
};
