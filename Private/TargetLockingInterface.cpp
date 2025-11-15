// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "TargetLockingInterface.h"

UTargetLockDelegateWrapper::UTargetLockDelegateWrapper()
{
}

void UTargetLockDelegateWrapper::CallStopTargetLockDelegate()
{
	StopTargetLockDelegate.Broadcast();
}
