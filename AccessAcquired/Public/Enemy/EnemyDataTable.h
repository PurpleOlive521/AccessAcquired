// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnemyDataTable.generated.h"

class AEnemyBase;

/**
 *
 */
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FEnemyRow : public FTableRowBase
{
	GENERATED_BODY()

	FEnemyRow() = default;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<AEnemyBase> Enemy;
};