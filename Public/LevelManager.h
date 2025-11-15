// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "LevelTypes.h"

#include "LevelManager.generated.h"

UCLASS()
class VERTICALSLICE_API ALevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelManager();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void UpdateExternalLevelState(TArray<ELevelName> InExternalLevels, bool InLoadExternalLevels);

	UFUNCTION(BlueprintCallable, CallInEditor)
	void ResetExternalLevelState();

protected:

	void LoadLevelState();

	// Iterative function that loops through all requested levels, loading each in order.
	void LoadLevelsBlocking(TArray<FName> LevelsToLoad);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bLoadExternalLevels = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ELevelName> ExternalLevels;

};
