// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "DialogueSystemTypes.h"

DEFINE_LOG_CATEGORY(LogDialogueSystem)


FDialogueRow::FDialogueRow(const FInnerVoiceRow& Data)
{
	this->DialogueName = Data.DialogueName;
	this->Tags = Data.Tags;
	this->Text = Data.Text;
}

void FDialogueRow::OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems)
{

}

bool FDialogueRow::IsDialogueEmpty() const
{
	FString CopyCharacter = Character;
	FString CopyPortrait = Portrait;
	FString CopyText = Text;

	CopyCharacter.RemoveSpacesInline();
	CopyPortrait.RemoveSpacesInline();
	CopyText.RemoveSpacesInline();

	const bool bIsAllEmpty = CopyCharacter.IsEmpty() && CopyPortrait.IsEmpty() && CopyText.IsEmpty();
	return bIsAllEmpty;
}

void FInnerVoiceRow::OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems)
{
}

bool FInnerVoiceRow::IsDialogueEmpty() const
{
	// Copy to avoid editing the row's data
	FString CopyText = Text;

	CopyText.RemoveSpacesInline();

	return CopyText.IsEmpty();
}


URecordedDialogue::URecordedDialogue()
{
}

URecordedDialogue::URecordedDialogue(FDialogueRow Dialogue)
{
	this->Dialogue = Dialogue;
}
