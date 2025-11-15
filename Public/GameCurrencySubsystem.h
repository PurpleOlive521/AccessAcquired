// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "SaveableObjectInterface.h"

#include "GameCurrencySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntelChangedSignature, int, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntelNotificationsSignature, int, Delta);

UCLASS()
class VERTICALSLICE_API UGameCurrencySubsystem : public UGameInstanceSubsystem, public ISaveableObjectInterface
{
	GENERATED_BODY()
	
public:
	// Begin ISaveableObjectInterface
	void SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject) override;

	void LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject) override;

	// End ISaveableObjectInterface

	UFUNCTION(BlueprintCallable, Category = "GameCurrency")
	void ChangeIntel(int Delta, bool bSendNotifications);

	UFUNCTION(BlueprintCallable, Category = "GameCurrency")
	void SetIntel(int NewValue, bool bSendNotifications);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameCurrency")
	int GetIntel();

private:
	void CheckHasFetchedSave();

	int CurrentIntel = 0;

	bool bHasFetchedSaveData = false;


public:
	// --- Delegates 

	// Broadcasted when a value change occurs, uses new Intel value as payload. Guaranteed to not be 0.
	UPROPERTY(BlueprintAssignable)
	FOnIntelChangedSignature OnIntelChangedDelegate;

	// Broadcasted to alert of special changes in Intel, uses Delta as payload. Guaranteed to not be 0.
	UPROPERTY(BlueprintAssignable)
	FIntelNotificationsSignature IntelNotificationsDelegate;
};
