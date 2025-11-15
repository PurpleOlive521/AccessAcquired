// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerControllerInterface.generated.h"

class UWidgetMenuStack;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerControllerInterface : public UInterface
{
	GENERATED_BODY()
};

class VERTICALSLICE_API IPlayerControllerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	// --- Camera Blending 

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerControllerInterface")
	void InitBlend();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerControllerInterface")
	void ToggleScreenMappingMovement(bool bEnable, bool bUsePure2DMovement, float BlendTime);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerControllerInterface")
	void EndCameraBlend(float BlendTime);

	// --- Player Input

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerControllerInterface")
	void ActivateUIInput(UUserWidget* WidgetToFocus);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerControllerInterface")
	void ActivateGameInput();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (ForceAsFunction), Category = "PlayerControllerInterface")
	void TogglePauseMenu();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (ForceAsFunction), Category = "PlayerControllerInterface")
	void ToggleAllPlayerInput(bool AllowInput);

	// --- UI

	// Get the MenuStack containing all game UI
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (ForceAsFunction), Category = "PlayerControllerInterface")
	UWidgetMenuStack* GetMenuStack() const;

	// Allows GameInstance and Level Blueprints to poll for the PlayerController
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerControllerInterface")
	bool HasInitialised() const;

	// --- Unlockable Skill System
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PlayerControllerInterface")
	class UUnlockableSkillSystem* GetUnlockableSkillSystem();

};
