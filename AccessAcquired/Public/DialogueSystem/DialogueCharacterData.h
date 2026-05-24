// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogueCharacterData.generated.h"

UCLASS(Blueprintable, HideDropdown)
class VERTICALSLICE_API UDialogueCharacterData : public UObject
{
	GENERATED_BODY()
	
public: 
	UDialogueCharacterData();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString Name = "Character Name";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap < FString, TSoftObjectPtr<UTexture2D> > Portraits;

};
