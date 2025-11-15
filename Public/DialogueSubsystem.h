// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "DialogueSystemTypes.h"
#include "DialogueSettings.h"

#include "DialogueSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnyDialogueFinishedSignature, FString, DialogueName);

/**
 * Subsystem responsible for creating quick-access maps of Dialogue data for easy retrieval during gameplay.
 *
 * Performs auditing on launch to alert of malformed data, missing DataTables, Character objects and more.
 * 
 * Relies on Soft references which are resolved during requests and when launch to avoid keeping all the data in memory at runtime. Results in slightly more overhead when requesting dialogues.
 */
UCLASS()
class VERTICALSLICE_API UDialogueSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// --- Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// --- End USubsystem Interface

	UFUNCTION(BlueprintCallable)
	void StartVisualNovelDialogue(const FString& DialogueName);

	UFUNCTION(BlueprintCallable)
	void StartInnerVoiceDialogue(const FString& DialogueName);

	// Returns true if the requested Dialogue was found.
	UFUNCTION(BlueprintCallable)
	bool RequestVisualNovelData(const FString& DialogueName, FPackagedDialogueSegment& OutData);

	// Returns true if the requested Dialogue was found.
	UFUNCTION(BlueprintCallable)
	bool RequestInnerVoiceData(const FString& DialogueName, FPackagedDialogueSegment& OutData);

	// Tries to get the Character by Character name. Returns nullptr if not found.
	TSubclassOf<UDialogueCharacterData> GetCharacterData(const FString& CharacterName);

	// Broadcasts the OnAnyDialogueFinishedDelegate. Used as payload on newly created dialogue widgets to forward their delegate through this Subsystem.
	UFUNCTION(BlueprintCallable)
	void OnCreatedDialogueFinished(FString DialogueName);

protected:

	// Starts the Subsystem. Multistep process that checks for any errors in the dialogue data before launching fully.
	// Returns false if any step fails.
	bool LaunchSystem();

	// Returns true if constructed correctly. Sets FailureMessage on failure.
	bool TryConstructDialogueMap();

	// Returns true if constructed correctly. Sets FailureMessage on failure.
	bool TryConstructInnerVoiceMap();

	// Returns true if the system finished auditing all Dialogue without any errors.
	bool AuditDialogueMap();

	// Returns true if the system finished auditing all InnerVoice without any errors.
	bool AuditInnerVoiceMap();

private:

	// Loads the DataTable, avoid frequent use. Returns null if not set in UDialogueSettings.
	inline const UDataTable* GetDialogueDataTable() const
	{
		return GetDefault<UDialogueSettings>()->DialogueSheet.LoadSynchronous();
	}

	// Loads the DataTable, avoid frequent use. Returns null if not set in UDialogueSettings.
	inline const UDataTable* GetInnerVoiceDataTable() const
	{
		return GetDefault<UDialogueSettings>()->InnerVoiceSheet.LoadSynchronous();
	}

	TMap<FString, FDialogueSegment> DialogueLookupMap;

	TMap<FString, FDialogueSegment> InnerVoiceLookupMap;
	
	// Set by any subprocess on failure during startup, to be reported in the log.
	FString FailureMessage;

public:
	// --- Delegates

	UPROPERTY(BlueprintAssignable)
	FOnAnyDialogueFinishedSignature OnAnyDialogueFinishedDelegate;
};
