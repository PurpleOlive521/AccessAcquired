// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ExtendedGameUserSettings.h"
#include "AAGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UAAGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "WorldContextObject"), Category = "AAGameInstance")
	static UAAGameInstance* GetAAGameInstance(const UObject* WorldContextObject);

	// Can return nullptr if class is not initialized yet, or not designated as a game settings mapper.
	UFUNCTION(BlueprintCallable, Category = "AAGameInstance")
	USettingsMapper* GetSettingsMapper(TSubclassOf<USettingsMapper> Class);

	UFUNCTION(BlueprintCallable, Category = "AAGameInstance")
	void GetAllSettingsMappers(TArray<USettingsMapper*>& OutMappers);

	USettingsMapper* InstantiateSettingsMapper(TSubclassOf<USettingsMapper> Class);

	void TryFirstTimeApplyingSettings();

protected:

	UPROPERTY(EditAnywhere, Category = "AAGameInstance")
	TArray<TSoftClassPtr<USettingsMapper>> SettingsTypes;

	UPROPERTY(Transient)
	TMap<TSubclassOf<USettingsMapper>, TObjectPtr<USettingsMapper>> ActiveSettingsMappers;
};
