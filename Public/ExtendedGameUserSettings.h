// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "ExtendedGameUserSettings.generated.h"

/**
 * An extension of the GameUserSettings with extra functionality tailored for Access Acquired
 */
UCLASS(config = GameUserSettings, configdonotcheckdefaults, Blueprintable)
class VERTICALSLICE_API UExtendedGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UExtendedGameUserSettings(const FObjectInitializer& ObjectInitializer);

	// Getter that returns the active ExtendedGameUserSettings. Casts on return, so cache reference if possible.
	UFUNCTION(BlueprintCallable, Category = Settings)
	static UExtendedGameUserSettings* GetExtendedGameUserSettings();

	// --- VFX

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetChromaticAberrationEnabled(bool bEnable);

	UFUNCTION(BlueprintPure, Category = Settings)
	bool IsChromaticAberrationEnabled() const;

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetCameraShakeEnabled(bool bEnable);

	UFUNCTION(BlueprintPure, Category = Settings)
	bool IsCameraShakeEnabled() const;

	// --- Audio

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetMasterVolume(float NewVolume);

	UFUNCTION(BlueprintPure, Category = Settings)
	float GetMasterVolume() const;

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetMusicVolume(float NewVolume);

	UFUNCTION(BlueprintPure, Category = Settings)
	float GetMusicVolume() const;

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetSFXVolume(float NewVolume);

	UFUNCTION(BlueprintPure, Category = Settings)
	float GetSFXVolume() const;

	// --- Controls

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetCameraSensitivity(float NewSensitivity);

	UFUNCTION(BlueprintPure, Category = Settings)
	float GetCameraSensitivity() const;

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetInvertYAxisEnabled(bool bEnable);

	UFUNCTION(BlueprintPure, Category = Settings)
	bool IsInvertYAxisEnabled() const;

	UFUNCTION(BlueprintCallable, Category = Settings)
	void SetGamepadRumbleEnabled(bool bEnable);

	UFUNCTION(BlueprintPure, Category = Settings)
	bool IsGamepadRumbleEnabled() const;


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
	bool bUseGamepadRumble = true;

};
