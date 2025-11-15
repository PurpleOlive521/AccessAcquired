// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetLockingInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopTargetLockDelegate);

// Hack to allow delegates to be used in interfaces
// Usually, since delegates are accessed the same as properties, a hard reference or casting would be
// required to bind to the delegate. Since the player binds to this delegate every time a new target is locked
// casting would be unnecessarily expensive, but grabbing a UObject containing the delegate circumvents the need for casting.
UCLASS(Blueprintable)
class VERTICALSLICE_API UTargetLockDelegateWrapper : public UObject 
{
	GENERATED_BODY()

public:
	UTargetLockDelegateWrapper();

	UFUNCTION(BlueprintCallable, Category = "TargetLockWrapper")
	void CallStopTargetLockDelegate();

	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, Category = "TargetLocking")
	FStopTargetLockDelegate StopTargetLockDelegate;
};



// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTargetLockingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VERTICALSLICE_API ITargetLockingInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TargetLocking")
	UTargetLockDelegateWrapper* GetStopTargetLockDelegateWrapper() const;

};
