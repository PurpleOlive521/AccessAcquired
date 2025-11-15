// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "AssetValidationData.generated.h"

USTRUCT(BlueprintType)

struct FAssetNameValidationData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FAssetNameValidationData();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AssetNameValidation")
	TSoftClassPtr<UObject> AssetType;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AssetNameValidation")
	TArray<FString> AllowedPrefix;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AssetNameValidation")
	TArray<FString> AllowedSuffix;

};
