// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameInstanceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGameInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VERTICALSLICE_API IGameInstanceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Called at the same time as the level Actors. Called from a BP_LevelManager
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GameInstanceInterface")
	void OnLevelBeginPlay();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GameInstanceInterface")
	class AAudioManager* GetAudioManager();
};
