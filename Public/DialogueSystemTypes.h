// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "DialogueCharacterData.h"

#include "DialogueSystemTypes.generated.h"

struct FInnerVoiceRow;

DECLARE_LOG_CATEGORY_EXTERN(LogDialogueSystem, Log, All)

// Macro for logging in the LogDialogueSystem category
#define DS_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogDialogueSystem, Verbosity, Format, ##__VA_ARGS__); \
}

// Which style the dialogue is meant to be displayed in. Put here for lack of a better centralised place. Used in DialogueTriggers
UENUM(BlueprintType)
enum class EDialogueType : uint8
{
	EDT_VisualNovel		UMETA(DisplayName = "Visual Novel"),
	EDT_InnerVoice		UMETA(DisplayName = "Inner Voice")
};

USTRUCT(BlueprintType)
struct FDialogueRow : public FTableRowBase
{
	GENERATED_BODY()

	FDialogueRow() {};

	FDialogueRow(const FInnerVoiceRow& Data);

	// --- Begin FTableRowBase Interface
	void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems) override;
	// --- End FTableRowBase Interface

	// Returns true if the struct does not contain dialogue meant to be displayed. Spaces count as empty.
	bool IsDialogueEmpty() const;

	// The name used when referencing a specific Dialogue. Should NOT be changed after being imported.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
	FString DialogueName = "";

	// Currently has no functionality, but would be used for special tags that encompass an entire line.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
	FString Tags = "";

	// The name of the speaker. Used to fetch data such as portrait & name to display alongside the text content.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
	FString Character = "";

	// The specific character art to display. 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
	FString Portrait = "";

	// The text to display in the dialogue box.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
	FString Text = "";
};

USTRUCT(BlueprintType)
struct FInnerVoiceRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FInnerVoiceRow() {};

	void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems) override;

	// Returns true if the struct does not contain dialogue meant to be displayed. Spaces count as empty.
	bool IsDialogueEmpty() const;

	// The name used when referencing a specific Dialogue. Should NOT be changed after being imported.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
	FString DialogueName = "";

	// Currently has no functionality, but would be used for special tags that encompass an entire line.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
	FString Tags = "";

	// The text to display in the dialogue box.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialogue")
	FString Text = "";

};

// A complete segment of dialogue, with start and end rows from the source DataTable.
USTRUCT(BlueprintType)
struct FDialogueSegment
{
	GENERATED_BODY()

	FDialogueSegment() {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int StartRow = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int EndRow = 0;
};

// A populated dialogue segment with the DialogueRows to display, as well as any needed CharacterData for the dialogue.
USTRUCT(BlueprintType)
struct FPackagedDialogueSegment
{
	GENERATED_BODY()

	FPackagedDialogueSegment() {};

	// The unique name for this dialogue.
	UPROPERTY(BlueprintReadOnly)
	FString DialogueName;

	// Dialogue lines
	UPROPERTY(BlueprintReadOnly)
	TArray<FDialogueRow> DialogueData;

	// The characters data used in the dialogue 
	UPROPERTY(BlueprintReadOnly)
	TArray<TSubclassOf<UDialogueCharacterData>> UsedCharacters;
};

// UObject container for a recorded dialogue. Passed onto ListView for dialogue history visualisation
UCLASS(BlueprintType)
class VERTICALSLICE_API URecordedDialogue : public UObject
{
	GENERATED_BODY()

public:
	URecordedDialogue();

	URecordedDialogue(FDialogueRow Dialogue);

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FDialogueRow Dialogue;
};