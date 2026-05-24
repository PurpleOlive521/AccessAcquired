// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SaveableObjectInterface.h"
#include "GateComponent.h"

#include "LockedDoor.generated.h"

UCLASS()
class VERTICALSLICE_API ALockedDoor : public AActor, public ISaveableObjectInterface
{
	GENERATED_BODY()
	
public:	
	ALockedDoor(const FObjectInitializer& ObjectInitializer);

	// --- Begin AActor Interface
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;
	// --- End AActor Interface

	// --- Begin SaveableObject Interface
	virtual bool OnSerialize(FSaveGameArchive& Archive, bool bIsLoading) override;
	// --- End SaveableObject Interface

	UFUNCTION(BlueprintCallable)
	UGateControllerComponent* GetGateController() const;

protected:

	UPROPERTY(EditAnywhere, Category = "LockedDoor")
	EGateState StartingState = EGateState::EGS_Closed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LockedDoor")
	TObjectPtr<UGateComponent> GateRight = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LockedDoor")
	TObjectPtr<UGateComponent> GateLeft = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LockedDoor")
	TObjectPtr<UGateControllerComponent> GateController = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LockedDoor")
	TObjectPtr<USceneComponent> CustomRoot = nullptr;
};
