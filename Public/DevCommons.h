// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

//
// Optional include meant for project-wide utility macros and types.
//

DECLARE_LOG_CATEGORY_EXTERN(VSLog, Log, All)

#define PrintToScreenError(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, text)
#define PrintToScreenSuccess(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, text)
#define PrintToScreen(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Silver, text)

#define ENDL TEXT("\n")

// Enum associated developers 
UENUM(BlueprintType)
enum class EDeveloperName: uint8 {

	EDN_Anna				UMETA(DisplayName = "Anna Hoffstedt"),
	EDN_Isabelle			UMETA(DisplayName = "Isabelle Bostrom"),
	EDN_Jamie				UMETA(DisplayName = "Jamie Braad Sernbo"),
	EDN_Manuel				UMETA(DisplayName = "Manuel Obermayr"),
	EDN_Meike				UMETA(DisplayName = "Meike Lenting"),
	EDN_Oliver				UMETA(DisplayName = "Oliver Osterlund Stare"),
};

namespace DebugTypes
{
	// Wraps the tag in the appropriate format for RichText, e.g '<TagName>'
	#define WRAP_TAG(str) TEXT("<" #str ">")

	static const FString TextTag_Default = WRAP_TAG(Default);
	static const FString TextTag_Italic = WRAP_TAG(Italic);
	static const FString TextTag_Bold = WRAP_TAG(Bold);
	static const FString TextTag_Header = WRAP_TAG(Header);
	static const FString TextTag_Accept = WRAP_TAG(Default_Green);
	static const FString TextTag_Warning = WRAP_TAG(Default_Red);
	static const FString TextTag_Highlight = WRAP_TAG(Default_Yellow);
	static const FString TextTag_End = WRAP_TAG(/ );
}