// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "GameplayTagSystem.h"

#include "ModeSwitchIconWidget.generated.h"


UCLASS(Blueprintable)
class VERTICALSLICE_API UModeSwitchIconWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	// --- Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// --- End UUserWidget Interface

	UFUNCTION()
	void OnGameplayTagChanged(FGameplayTag ChangedTag, int Count);

	// Toggles the icon, to show whether this mode has been unlocked yet or not. True means that it's unlocked, false means locked.
	UFUNCTION(BlueprintCallable, Category = "ModeSwitchIcon")
	void ToggleIconUnlocked(bool bUnlocked);

protected:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ModeSwitchIcon")
	void SetIconUnlocked();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ModeSwitchIcon")
	void SetIconLocked();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ModeSwitchIcon")
	FGameplayTag ModeTag;

	UPROPERTY(BlueprintReadOnly, Category = "ModeSwitchIcon")
	bool bIsModeUnlocked = false;
};
