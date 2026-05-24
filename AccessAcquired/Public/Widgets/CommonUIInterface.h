// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CommonUIInterface.generated.h"

class UWidgetMenuStack;

UINTERFACE(MinimalAPI)
class UCommonUIInterface : public UInterface
{
	GENERATED_BODY()
};

class VERTICALSLICE_API ICommonUIInterface
{
	GENERATED_BODY()
	
public:

	// Allows any Widget to access the MenuStack that it is part of.
	// From there additional widgets can be pushed, removed and modified.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CommonUIInterface")
	void SetParentMenuStack(UWidgetMenuStack* MenuStack);

};
