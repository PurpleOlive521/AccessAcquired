// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SaveableObjectInterface.h"
#include "GuidTag.h"

#include "DialogueTrigger.generated.h"

UCLASS()
class VERTICALSLICE_API ADialogueTrigger : public AActor, public ISaveableObjectInterface
{
	GENERATED_BODY()
	
public:	
	ADialogueTrigger();

	// --- Begin AActor Interface
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// --- End AActor Interface

	// Ensures that the ID is not copied over when the Actor is duplicated in the Editor.
	virtual void PostEditImport() override;

	// --- Begin ISaveableObjectInterface
	virtual void SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject) override;

	virtual void LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject) override;
	// --- End ISaveableObjectInterface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGuidTag Id;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bPreviouslyTriggered = false;
};
