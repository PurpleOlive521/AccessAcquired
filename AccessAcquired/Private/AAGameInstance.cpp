// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AAGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UAAGameInstance::Init()
{
	Super::Init();

	TryFirstTimeApplyingSettings();
	
	TArray<USettingsMapper*> Settings;
	GetAllSettingsMappers(Settings);

	// Apply any settings that arent handled automatically by GameUserSettings
	for (auto& Setting : Settings)
	{
		Setting->ApplySetting();
	}
}

UAAGameInstance* UAAGameInstance::GetAAGameInstance(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		return Cast<UAAGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	}

	return nullptr;
}

USettingsMapper* UAAGameInstance::GetSettingsMapper(TSubclassOf<USettingsMapper> Class)
{
	if (not Class)
	{
		return nullptr;
	}

	if (ActiveSettingsMappers.Contains(Class))
	{
		return ActiveSettingsMappers.FindChecked(Class);
	}

	USettingsMapper* NewSettingsMapper = InstantiateSettingsMapper(Class);

	return NewSettingsMapper;
}

void UAAGameInstance::GetAllSettingsMappers(TArray<USettingsMapper*>& OutMappers)
{
	OutMappers.Empty();

	for (const auto& Class : SettingsTypes)
	{
		if (UClass* DerefClass = Class.LoadSynchronous())
		{
			OutMappers.Add(GetSettingsMapper(DerefClass));
		}
	}
}

USettingsMapper* UAAGameInstance::InstantiateSettingsMapper(TSubclassOf<USettingsMapper> Class)
{
	if (ActiveSettingsMappers.Contains(Class))
	{
		return ActiveSettingsMappers.FindChecked(Class);
	}

	USettingsMapper* NewSettingsMapper = NewObject<USettingsMapper>(this, Class);
	NewSettingsMapper->Initialize();
	ActiveSettingsMappers.Add(Class, NewSettingsMapper);

	return NewSettingsMapper;
}

void UAAGameInstance::TryFirstTimeApplyingSettings()
{
	if (UExtendedGameUserSettings* GameSettings = UExtendedGameUserSettings::Get())
	{
		if (GameSettings->ShouldGenerateNewOnLoad())
		{
			GameSettings->ApplyFirstTimeSettings();
			GameSettings->SetGenerateNewOnLoad(false /* bGenerateNew */);
		}
	}
}
