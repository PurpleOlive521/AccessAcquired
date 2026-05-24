// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "SaveableObjectInterface.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayMessages/GameplayMessageTypes.h"

#include "GameCurrencySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntelChangedSignature, int, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIntelNotificationsSignature, int, Delta);

UCLASS()
class VERTICALSLICE_API UGameCurrencySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// --- Begin SaveableObject Interface
	bool OnSerialize(FSaveGameArchive& Archive, bool bIsLoading);
	// --- End SaveableObject Interface

	// Helper getter
	static UGameCurrencySubsystem* Get(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, Category = "GameCurrency")
	void ChangeIntel(int Delta, bool bSendNotifications);

	UFUNCTION(BlueprintCallable, Category = "GameCurrency")
	void SetIntel(int NewValue, bool bSendNotifications);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameCurrency")
	int GetIntel() const;

	void ReceivedReward(FGameplayTag Channel, const FAARewardMessage& Reward);

protected:

	UPROPERTY(SaveGame)
	int32 CurrentIntel = 0;

private:

	FGameplayMessageListenerHandle MessageListenerHandle;

public:
	// --- Delegates 

	// Broadcasted when a value change occurs, uses new Intel value as payload.
	UPROPERTY(BlueprintAssignable)
	FOnIntelChangedSignature OnIntelChangedDelegate;

	// Broadcasted to alert of special changes in Intel, uses Delta as payload.
	UPROPERTY(BlueprintAssignable)
	FIntelNotificationsSignature IntelNotificationsDelegate;
};
