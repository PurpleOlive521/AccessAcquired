// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AAHUD.generated.h"

class UWidgetMenuStack;
class UConfirmationMenuWidget;

constexpr int32 MENU_STACK_ZORDER = 0;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API AAAHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AAAHUD() = default;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContext"))
	static AAAHUD* Get(const UObject* WorldContext);

	// Static that sources the MenuStack from the PlayerControllers HUD. Don't call frequently, cache result if possible.
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContext"))
	static UWidgetMenuStack* GetMenuStack(const UObject* WorldContext);

	// The root Widget where all game UI layers are found, in which widgets should be added.
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContext"))
	UWidgetMenuStack* GetOrCreateMenuStack(const UObject* WorldContext);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TSubclassOf<UConfirmationMenuWidget> GetConfirmationWidgetClass() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UWidgetMenuStack> DefaultWidgetMenuStackClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UConfirmationMenuWidget> DefaultConfirmationWidgetClass = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetMenuStack> MenuStackWidget = nullptr;
};
