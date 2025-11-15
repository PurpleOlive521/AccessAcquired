// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "DialogueSubsystem.h"

#include "DevCommons.h"
#include "DialogueSystemTypes.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerControllerInterface.h"
#include "WidgetMenuStack.h"
#include "VisualNovelWidget.h"
#include "InnerVoiceWidget.h"


void UDialogueSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	const bool bLaunchedSuccessfully = LaunchSystem();
	if (!bLaunchedSuccessfully)
	{
		PrintToScreenError("DialogueSubsystem launch terminated due to errors. See Output Log for more details.");
	}
}

void UDialogueSubsystem::Deinitialize()
{
}

bool UDialogueSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    // Prevents servers or other instances from initializing this subsystem
    const UGameInstance* GameInstance = CastChecked<UGameInstance>(Outer);
    const bool bIsServerWorld = GameInstance->IsDedicatedServerInstance();
    return !bIsServerWorld;
}

void UDialogueSubsystem::StartVisualNovelDialogue(const FString& DialogueName)
{
	FPackagedDialogueSegment ReceivedPackagedDialogue;
	const bool bReceivedValidData = RequestVisualNovelData(DialogueName, ReceivedPackagedDialogue);

	if (!bReceivedValidData)
	{
		DS_LOG(Error, TEXT("VisualNovelDialogue not found: %s."), *DialogueName);
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (UWidgetMenuStack* MenuStack = IPlayerControllerInterface::Execute_GetMenuStack(PlayerController))
	{
		const UDialogueSettings* Settings = GetDefault<UDialogueSettings>();
		UClass* DialogueWidgetClass = Settings->DialogueWidget.LoadSynchronous();

		UVisualNovelWidget* DialogueWidget = MenuStack->PushToGame<UVisualNovelWidget>(DialogueWidgetClass);

		// Make sure we are notified when the dialogue ends
		DialogueWidget->OnDialogueFinishedDelegate.AddUniqueDynamic(this, &UDialogueSubsystem::OnCreatedDialogueFinished);

		DialogueWidget->QueueDialogue_Implementation(ReceivedPackagedDialogue);
		DialogueWidget->Execute_StartDialogue(DialogueWidget);

		IPlayerControllerInterface::Execute_ActivateUIInput(PlayerController, DialogueWidget);
	}
}

void UDialogueSubsystem::StartInnerVoiceDialogue(const FString& DialogueName)
{
	FPackagedDialogueSegment ReceivedPackagedDialogue;
	const bool bReceivedValidData = RequestInnerVoiceData(DialogueName, ReceivedPackagedDialogue);

	if (!bReceivedValidData)
	{
		DS_LOG(Error, TEXT("VisualNovelDialogue not found: %s."), *DialogueName)
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	IPlayerControllerInterface* PlayerControllerInterface = Cast<IPlayerControllerInterface>(PlayerController);
	if (!PlayerControllerInterface)
	{
		DS_LOG(Error, TEXT("Could not access PlayerController or it's interface."))
			return;
	}

	if (UWidgetMenuStack* MenuStack = PlayerControllerInterface->Execute_GetMenuStack(PlayerController))
	{
		const UDialogueSettings* Settings = GetDefault<UDialogueSettings>();
		UClass* InnerVoiceWidgetClass = Settings->InnerVoiceWidget.LoadSynchronous();
 
		UInnerVoiceWidget* InnerVoiceWidget = MenuStack->PushToGame<UInnerVoiceWidget>(InnerVoiceWidgetClass);

		// Make sure we are notified when the dialogue ends
		InnerVoiceWidget->OnDialogueFinishedDelegate.AddUniqueDynamic(this, &UDialogueSubsystem::OnCreatedDialogueFinished);

		InnerVoiceWidget->QueueDialogue(ReceivedPackagedDialogue);
		InnerVoiceWidget->StartDialogue();

		PlayerControllerInterface->Execute_ActivateUIInput(PlayerController, InnerVoiceWidget);
	}
}

bool UDialogueSubsystem::TryConstructDialogueMap()
{
	const UDataTable* DialogueDataTable = GetDialogueDataTable();
	if (!DialogueDataTable)
	{
		DS_LOG(Error, TEXT("No Dialogue DataTable provided, terminating launch. Verify that a valid DataTable is selected in DialogueSettings!"));
		return false;
	}

	// DataTables RowName are 1 indexed, so we must match that
	int CurrentRowIndex = 1;

	// Setup state machine for reading dialogue segments
	FString InternalDataKey = "";
	FDialogueSegment InternalData = {};
	bool bNewDataStarted = false;

	for (const auto& Row : DialogueDataTable->GetRowMap())
	{
		const FDialogueRow* RowValue = reinterpret_cast<FDialogueRow*>(Row.Value);
		const FName& RowKey = Row.Key;

		// Ensure that the rows are indexed correctly, as it can otherwise invalidate the stored start/end row numbers in FDialogueSegment
		if(RowKey != FName(*FString::Printf(TEXT("%i"), CurrentRowIndex)))
		{
			FailureMessage = FString::Printf(TEXT("Row '%s' in VisualNovelDialogue is not indexed correctly, expected '%s'."), *RowKey.ToString(), *FString::Printf(TEXT("%i"), CurrentRowIndex));
			return false;
		}

		// Found a dialogue name, assume start of dialogue segment
		if (RowValue->DialogueName != "")
		{
			// New segment started before the previous one ended
			if (bNewDataStarted)
			{
				FailureMessage = TEXT("VisualNovelDialogue segment started before previous one ended at %i."), CurrentRowIndex;
				return false;
			}

			InternalDataKey = RowValue->DialogueName;
			InternalData.StartRow = CurrentRowIndex;
			bNewDataStarted = true;
		}

		// Segment ends are denoted by an empty row at some point after a segment has started
		const bool bValidDialogueEnd = bNewDataStarted && RowValue->IsDialogueEmpty();
		if (bValidDialogueEnd)
		{
			// The actual last row of the segment was the previous row
			InternalData.EndRow = CurrentRowIndex - 1; 
			DialogueLookupMap.Add(InternalDataKey, InternalData);
			bNewDataStarted = false;
		}

		CurrentRowIndex++;
	}

	return true;
}

bool UDialogueSubsystem::TryConstructInnerVoiceMap()
{
	const UDataTable* InnerVoiceDataTable = GetInnerVoiceDataTable();
	if (!InnerVoiceDataTable)
	{
		FailureMessage = TEXT("No InnerVoice DataTable provided, terminating launch. Verify that a valid DataTable is selected in DialogueSettings!");
		return false;
	}

	// DataTables RowName are 1 indexed, so we must match that
	int CurrentRowIndex = 1;

	// Setup state machine for reading dialogue segments
	FString InternalDataKey = "";
	FDialogueSegment InternalData = {};
	bool bNewDataStarted = false;

	for (const auto& Row : InnerVoiceDataTable->GetRowMap())
	{
		const FInnerVoiceRow* RowValue = reinterpret_cast<FInnerVoiceRow*>(Row.Value);
		const FName& RowKey = Row.Key;

		// Ensure that the rows are indexed correctly, as it can otherwise invalidate the stored start/end row numbers in FDialogueSegment
		if (RowKey != FName(*FString::Printf(TEXT("%i"), CurrentRowIndex)))
		{
			FailureMessage = FString::Printf(TEXT("Row '%s' in InnerVoiceDialogue is not indexed correctly, expected '%s'."), *RowKey.ToString(), *FString::Printf(TEXT("%i"), CurrentRowIndex));
			return false;
		}

		// Found a dialogue name, start of dialogue segment
		if (RowValue->DialogueName != "")
		{
			// New segment started before the previous one ended
			if (bNewDataStarted)
			{
				FailureMessage = TEXT("Inner Voice segment started before previous one ended at %i."), CurrentRowIndex;
				return false;
			}

			InternalDataKey = RowValue->DialogueName;
			InternalData.StartRow = CurrentRowIndex;
			bNewDataStarted = true;
		}

		// Segment ends are denoted by an empty row at some point after a segment has started
		const bool bValidDialogueEnd = bNewDataStarted && RowValue->IsDialogueEmpty();
		if (bValidDialogueEnd)
		{
			InternalData.EndRow = CurrentRowIndex - 1;
			InnerVoiceLookupMap.Add(InternalDataKey, InternalData);
			bNewDataStarted = false;
		}

		CurrentRowIndex++;
	}

	return true;
}

bool UDialogueSubsystem::AuditDialogueMap()
{
	const UDataTable* DialogueDataTable = GetDialogueDataTable();

	// Allows it to continue auditing and reporting errors after encountering ones until the auditing is done.
	bool bFailedCheck = false;

	// Check for empty dialogue segments
	for (auto It : DialogueLookupMap)
	{
		FDialogueSegment Data = It.Value;
		if (Data.StartRow == 0 || Data.EndRow == 0)
		{
			bFailedCheck = true;
			DS_LOG(Error, TEXT("Dialogue segment '%s' is missing start or end row."), *It.Key);
		}
	}

	for (auto& It : DialogueLookupMap)
	{
		// Check for missing lines or characters in the stored dialogue segments
		FDialogueSegment Data = It.Value;
		for (int i = Data.StartRow; i <= Data.EndRow; i++)
		{
			FString RowString = FString::Printf(TEXT("%i"), i);
			FDialogueRow* Row = DialogueDataTable->FindRow<FDialogueRow>(FName(*RowString), "DialogueSubsystem");

			if (Row == nullptr)
			{
				bFailedCheck = true;
				DS_LOG(Error, TEXT("Dialogue segment '%s' is referencing non-existent row in DataTable."), *It.Key);
				continue;
			}

			TSubclassOf<UDialogueCharacterData> Character = GetCharacterData(Row->Character);
			if (Character == nullptr)
			{
				bFailedCheck = true;
				DS_LOG(Error, TEXT("Dialogue segment '%s' is missing or contains incorrect character data."), *It.Key);
				continue;
			}
		}
	}

	if (bFailedCheck == true)
	{
		return false;
	}

	return true;
}

bool UDialogueSubsystem::AuditInnerVoiceMap()
{
	// Allows it to continue auditing and reporting errors after encountering ones until the auditing is done.
	bool bFailedCheck = false;

	// Check for empty dialogue segments
	for (auto It : DialogueLookupMap)
	{
		FDialogueSegment Data = It.Value;
		if (Data.StartRow == 0 || Data.EndRow == 0)
		{
			bFailedCheck = true;
			DS_LOG(Error, TEXT("Inner Voice segment '%s' is missing start or end row."), *It.Key);
		}
	}

	if (bFailedCheck == true)
	{
		return false;
	}

	return true;
}

bool UDialogueSubsystem::LaunchSystem()
{
	const UDialogueSettings* Settings = GetDefault<UDialogueSettings>();
	if (Settings->CharacterData.IsEmpty())
	{
		DS_LOG(Error, TEXT("No CharacterData in UDialogueSettings, terminating launch. Verify that any CharacterData is added in DialogueSettings!"));
		return false;
	}

	if (!TryConstructDialogueMap())
	{
		DS_LOG(Error, TEXT("Failed to construct DialogueMap with the following error: "));
		DS_LOG(Error, TEXT("%s"), *FailureMessage);
		return false;
	}

	if (!TryConstructInnerVoiceMap())
	{
		DS_LOG(Error, TEXT("Failed to construct InnerVoiceMap with the following error: "));
		DS_LOG(Error, TEXT("%s"), *FailureMessage);
		return false;
	}

	if (!AuditDialogueMap() && !AuditInnerVoiceMap())
	{
		// They print their own failure messages, so no need to announce anything incase they fail
		return false;
	}

	// --- Success!

	const FString SuccessText1 = FString::Printf(TEXT("DialogueSubsystem: > Total Dialogues: %i > Total InnerVoices: %i"), DialogueLookupMap.Num(), InnerVoiceLookupMap.Num());
	const FString SuccessText2 = "DialogueSubsystem: DialogueSubsystem launched successfully!";

	PrintToScreenSuccess(SuccessText1);
	PrintToScreenSuccess(SuccessText2);
	DS_LOG(Display, TEXT("%s"), *SuccessText2);
	DS_LOG(Display, TEXT("%s"), *SuccessText1);

	return true;
}

TSubclassOf<UDialogueCharacterData> UDialogueSubsystem::GetCharacterData(const FString& CharacterName)
{
	const UDialogueSettings* Settings = GetDefault<UDialogueSettings>();

	for (auto const& It : Settings->CharacterData)
	{
		FString NameCopy = It->GetDefaultObject<UDialogueCharacterData>()->Name;
		UDialogueCharacterData* DefaultObject = It->GetDefaultObject<UDialogueCharacterData>();

		if (DefaultObject->Name == CharacterName)
		{
			return It;
		}
	}

	return nullptr;
}

void UDialogueSubsystem::OnCreatedDialogueFinished(FString DialogueName)
{
	OnAnyDialogueFinishedDelegate.Broadcast(DialogueName);
}

bool UDialogueSubsystem::RequestVisualNovelData(const FString& DialogueName, FPackagedDialogueSegment& OutData)
{
	// Blueprint does not seem to reset OutData between calls so we reset it first
	OutData.DialogueData.Empty();
	OutData.UsedCharacters.Empty();

	// Checking if the requested data exists
	FDialogueSegment* DialogueData = DialogueLookupMap.Find(DialogueName);

	if (DialogueData == nullptr)
	{
		DS_LOG(Error, TEXT("Requested Visual Novel dialogue not found: %s."), *DialogueName);
		return false;
	}

	const UDataTable* DialogueDataTable = GetDialogueDataTable();

	// Collecting all dialogue lines and characters
	for (int i = DialogueData->StartRow; i <= DialogueData->EndRow; i++)
	{
		// Silently get each line
		FString RowString = FString::Printf(TEXT("%i"), i);
		FDialogueRow* Row = DialogueDataTable->FindRow<FDialogueRow>(FName(*RowString), "DialogueSubsystem", false);

		// No Dialogue found
		if (Row == nullptr)
		{
			DS_LOG(Error, TEXT("Dialogue segment '%s' is missing row data during request packaging."), *DialogueName);
			return false;
		}

		OutData.DialogueData.Emplace(*Row);

		// Collecting character data
		TSubclassOf<UDialogueCharacterData> Character = GetCharacterData(Row->Character);

		// No Character found
		if (Character == nullptr)
		{
			DS_LOG(Error, TEXT("Dialogue segment '%s' is missing or contains incorrect character data during request packaging."), *DialogueName);
			return false;
		}

		OutData.UsedCharacters.AddUnique(Character);
	}

	OutData.DialogueName = DialogueName;
	return true;
}

bool UDialogueSubsystem::RequestInnerVoiceData(const FString& DialogueName, FPackagedDialogueSegment& OutData)
{
	// Blueprint does not seem to reset OutData between calls so we reset it first
	OutData.DialogueData.Empty();
	OutData.UsedCharacters.Empty();

	// Checking if the requested data exists
	FDialogueSegment* DialogueData = InnerVoiceLookupMap.Find(DialogueName);
	if (DialogueData == nullptr)
	{
		DS_LOG(Error, TEXT("Requested Inner Voice dialogue not found."), *DialogueName);
		return false;
	}

	const UDataTable* InnerVoiceDataTable = GetInnerVoiceDataTable();

	// Collecting all dialogue lines and characters
	for (int i = DialogueData->StartRow; i <= DialogueData->EndRow; i++)
	{
		// Silently get each line
		FString RowString = FString::Printf(TEXT("%i"), i);
		FInnerVoiceRow* Row = InnerVoiceDataTable->FindRow<FInnerVoiceRow>(FName(*RowString), "DialogueSubsystem", false);

		// No Dialogue found
		if (Row == nullptr)
		{
			DS_LOG(Error, TEXT("Inner Voice segment '%s' is missing row data during request packaging."), *DialogueName);
			return false;
		}

		OutData.DialogueData.Add(*Row);
	}

	OutData.DialogueName = DialogueName;
	return true;
}
