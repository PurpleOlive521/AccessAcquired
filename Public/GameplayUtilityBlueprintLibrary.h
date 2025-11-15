// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "CombatTypes.h"
#include "LevelTypes.h"
#include "GameplayTagContainer.h"

#include "GameplayUtilityBlueprintLibrary.generated.h"


UCLASS()
class VERTICALSLICE_API UGameplayUtilityBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// --- GameplayTags

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility|GameplayTags")
	static FGameplayTag GetModeAsGameplayTag(const EDamageType& Mode);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility|GameplayTags")
	static FGameplayTag GetAvailableModeAsGameplayTag(const EDamageType& Mode);

	// --- Etc.

	// Avoids using direct references to level objects or string literals for fetching said objects.
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility")
	static FName ConvertLevelToName(ELevelName Level);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility")
	static FString GetRandomLetter();

	// Returns the corresponding EAttributeType weakness for a given EDamageType.
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility")
	static EAttributeType GetCorrespondingWeakness(const EDamageType& DamageType);

};
