// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "BTService_IsTargetValid.generated.h"

class AAIController;

/**
 * 
 */
UCLASS(Blueprintable)
class VERTICALSLICE_API UBTService_IsTargetValid : public UBTService_BlueprintBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "BTD_IsTargetValid")
	bool IsPointReachable(AAIController* Controller, const FVector& Location);

	// "None" will result in default filter being used 
	UPROPERTY(EditAnywhere, Category = "BTD_IsTargetValid")
	TSubclassOf<UNavigationQueryFilter> FilterClass = nullptr;
};
