// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerControllerInterface.generated.h"

class UWidgetMenuStack;

UINTERFACE(MinimalAPI)
class UPlayerControllerInterface : public UInterface
{
	GENERATED_BODY()
};

class VERTICALSLICE_API IPlayerControllerInterface
{
	GENERATED_BODY()

public:
	
	// --- Camera Blending 
	
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
};
