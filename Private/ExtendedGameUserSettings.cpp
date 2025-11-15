// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "ExtendedGameUserSettings.h"

UExtendedGameUserSettings::UExtendedGameUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UExtendedGameUserSettings* UExtendedGameUserSettings::GetExtendedGameUserSettings()
{
	return Cast<UExtendedGameUserSettings>(UGameUserSettings::GetGameUserSettings());
}

// --- VFX

void UExtendedGameUserSettings::SetChromaticAberrationEnabled(bool bEnable)
{
	bUseChromaticAberration = bEnable;
}

bool UExtendedGameUserSettings::IsChromaticAberrationEnabled() const
{
	return bUseChromaticAberration;
}

void UExtendedGameUserSettings::SetCameraShakeEnabled(bool bEnable)
{
	bUseCameraShake = bEnable;
}

bool UExtendedGameUserSettings::IsCameraShakeEnabled() const
{
	return bUseCameraShake;
}

// --- Audio

void UExtendedGameUserSettings::SetMasterVolume(float NewVolume)
{
	MasterVolume = NewVolume;
}

float UExtendedGameUserSettings::GetMasterVolume() const
{
	return MasterVolume;
}

void UExtendedGameUserSettings::SetMusicVolume(float NewVolume)
{
	MusicVolume = NewVolume;
}

float UExtendedGameUserSettings::GetMusicVolume() const
{
	return MusicVolume;
}

void UExtendedGameUserSettings::SetSFXVolume(float NewVolume)
{
	SFXVolume = NewVolume;
}

float UExtendedGameUserSettings::GetSFXVolume() const
{
	return SFXVolume;
}

// --- Controls

void UExtendedGameUserSettings::SetCameraSensitivity(float NewSensitivity)
{
	CameraSensitivity = NewSensitivity;
}

float UExtendedGameUserSettings::GetCameraSensitivity() const
{
	return CameraSensitivity;
}

void UExtendedGameUserSettings::SetInvertYAxisEnabled(bool bEnable)
{
	bUseInvertYAxis = bEnable;
}

bool UExtendedGameUserSettings::IsInvertYAxisEnabled() const
{
	return bUseInvertYAxis;
}

void UExtendedGameUserSettings::SetGamepadRumbleEnabled(bool bEnable)
{
	bUseGamepadRumble = bEnable;
}

bool UExtendedGameUserSettings::IsGamepadRumbleEnabled() const
{
	return bUseGamepadRumble;
}
