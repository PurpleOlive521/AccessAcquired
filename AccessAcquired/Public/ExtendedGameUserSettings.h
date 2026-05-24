// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "SettingsMapper.h"
#include "ExtendedGameUserSettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsChangedSignature);

/**
 * An extension of the GameUserSettings with extra functionality tailored for Access Acquired
 */
UCLASS(config = GameUserSettings, configdonotcheckdefaults, Blueprintable, meta = (DisplayName = "Ingame Settings"))
class VERTICALSLICE_API UExtendedGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UExtendedGameUserSettings(const FObjectInitializer& ObjectInitializer);

	// Getter that returns the active ExtendedGameUserSettings. Casts on return, so cache results if possible.
	UFUNCTION(BlueprintCallable, Category = "ExtendedSettings")
	static UExtendedGameUserSettings* GetExtendedGameUserSettings();

	static UExtendedGameUserSettings* Get();

	// --- VFX

	UFUNCTION(BlueprintCallable, Category = "ExtendedSettings")
	void SetChromaticAberrationEnabled(bool bEnable);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExtendedSettings")
	bool IsChromaticAberrationEnabled() const;

	UFUNCTION(BlueprintCallable, Category = "ExtendedSettings")
	void SetCameraShakeEnabled(bool bEnable);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExtendedSettings")
	bool IsCameraShakeEnabled() const;

	// --- Audio

	UFUNCTION(BlueprintCallable, Category = "ExtendedSettings")
	void SetMasterVolume(float NewVolume);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExtendedSettings")
	float GetMasterVolume() const;

	UFUNCTION(BlueprintCallable, Category = "ExtendedSettings")
	void SetMusicVolume(float NewVolume);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExtendedSettings")
	float GetMusicVolume() const;

	UFUNCTION(BlueprintCallable, Category = "ExtendedSettings")
	void SetSFXVolume(float NewVolume);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExtendedSettings")
	float GetSFXVolume() const;

	// --- Controls

	UFUNCTION(BlueprintCallable, Category = "ExtendedSettings")
	void SetCameraSensitivity(float NewSensitivity);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExtendedSettings")
	float GetCameraSensitivity() const;

	UFUNCTION(BlueprintCallable, Category = "ExtendedSettings")
	void SetInvertYAxisEnabled(bool bEnable);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExtendedSettings")
	bool IsInvertYAxisEnabled() const;

	UFUNCTION(BlueprintCallable, Category = "ExtendedSettings")
	void SetInvertXAxisEnabled(bool bEnable);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExtendedSettings")
	bool IsInvertXAxisEnabled() const;

	UFUNCTION(BlueprintCallable, Category = "ExtendedSettings")
	void SetGamepadRumbleEnabled(bool bEnable);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExtendedSettings")
	bool IsGamepadRumbleEnabled() const;

	UFUNCTION(BlueprintCallable, Category = "ExtendedSettings")
	void SetGenerateNewOnLoad(bool bGenerateNew);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ExtendedSettings")
	bool ShouldGenerateNewOnLoad() const;

	// Based on some common platform properties.
	void ApplyFirstTimeSettings();

protected:

	// --- VFX

	UPROPERTY(config)
	bool bUseChromaticAberration = true;

	UPROPERTY(config)
	bool bUseCameraShake = true;

	// --- Audio

	UPROPERTY(config)
	float MasterVolume = 1;

	UPROPERTY(config)
	float MusicVolume = 1;

	UPROPERTY(config)
	float SFXVolume = 1;

	// --- Controls

	UPROPERTY(config)
	float CameraSensitivity = 1;

	UPROPERTY(config)
	bool bUseInvertYAxis = false;

	UPROPERTY(config)
	bool bUseInvertXAxis = false;

	UPROPERTY(config)
	bool bUseGamepadRumble = true;

	UPROPERTY(config)
	bool bGenerateNewOnLoad = true;

public:
	// --- Delegates

	// Broadcasted when any property is changed.
	UPROPERTY(BlueprintAssignable, Category = "ExtendedSettings")
	FOnSettingsChangedSignature OnSettingsChangedDelegate;
};
