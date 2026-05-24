// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "ExtendedGameUserSettings.h"
#include "Kismet/GameplayStatics.h"

UExtendedGameUserSettings::UExtendedGameUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UExtendedGameUserSettings* UExtendedGameUserSettings::GetExtendedGameUserSettings()
{
	return Get();
}

UExtendedGameUserSettings* UExtendedGameUserSettings::Get()
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

void UExtendedGameUserSettings::SetInvertXAxisEnabled(bool bEnable)
{
	bUseInvertXAxis = bEnable;
}

bool UExtendedGameUserSettings::IsInvertXAxisEnabled() const
{
	return bUseInvertXAxis;
}

void UExtendedGameUserSettings::SetGamepadRumbleEnabled(bool bEnable)
{
	bUseGamepadRumble = bEnable;
}

bool UExtendedGameUserSettings::IsGamepadRumbleEnabled() const
{
	return bUseGamepadRumble;
}

void UExtendedGameUserSettings::SetGenerateNewOnLoad(bool bGenerateNew)
{
	bGenerateNewOnLoad = bGenerateNew;
}

bool UExtendedGameUserSettings::ShouldGenerateNewOnLoad() const
{
	return bGenerateNewOnLoad;
}

void UExtendedGameUserSettings::ApplyFirstTimeSettings()
{
	ResolutionSizeX = 1920;
	ResolutionSizeY = 1080;
	FullscreenMode = 0; // Fullscreen

	SetOverallScalabilityLevel(3); // High Quality

	FrameRateLimit = 60;

	bUseVSync = true;

	if (FPlatformMisc::GetEnvironmentVariable(TEXT("SteamDeck")).Equals(FString(TEXT("1"))))
	{
		ResolutionSizeX = 1280;
		ResolutionSizeY = 800;
	}

	ApplySettings(true /* bCheckForCommandLineOverrides */);
}
