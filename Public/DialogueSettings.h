// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DialogueSettings.generated.h"

class UDialogueCharacterData;
class UCommonActivatableWidget;

/**
 * Settings for the Dialogue Subsystem to use.
 * Exposes Blueprint-only widgets and values to the Subsystem, since Subsystems can't be subclassed in Blueprint.
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Dialogue System"))
class VERTICALSLICE_API UDialogueSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	// The Blueprint Widget to load for the default Dialogue UI.
	UPROPERTY(Config, EditAnywhere)
	TSoftClassPtr<UCommonActivatableWidget> DialogueWidget;

	//, meta = (MetaClass = "/Script/UMG.UserWidget")

	// The Blueprint Widget to load for the inner-voice UI variation.
	UPROPERTY(Config, EditAnywhere)
	TSoftClassPtr<UCommonActivatableWidget> InnerVoiceWidget;

	UPROPERTY(Config, EditAnywhere, Category = "Data Source")
	TSoftObjectPtr<UDataTable> DialogueSheet;

	UPROPERTY(Config, EditAnywhere, Category = "Data Source")
	TSoftObjectPtr<UDataTable> InnerVoiceSheet;

	// TODO: Try to make into a soft pointer, so as to not pollute runtime with infrequently used data?
	UPROPERTY(Config, EditAnywhere, Category = "Data Source")
	TArray<TSubclassOf<UDialogueCharacterData>> CharacterData;

};
