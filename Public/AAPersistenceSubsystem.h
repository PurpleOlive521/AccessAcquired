// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayPersistenceSubSystem.h"

#include "LevelTypes.h"

#include "AAPersistenceSubsystem.generated.h"


UCLASS()
class VERTICALSLICE_API UAAPersistenceSubsystem : public UGameplayPersistenceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void CreateSaveGameObject() override;

private:
	// Inserted when creating a new save to ensure that the starting sublevel is requested
	ELevelName DefaultLevel = ELevelName::ELN_Backstreets_StartArea;

};
