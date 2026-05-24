// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExtendedCommonActivatableWidget.h"
#include "PlayerHudWidget.generated.h"

class UGameplaySystemComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UPlayerHudWidget : public UExtendedCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	// --- Begin CommonActivatableWidget Interface
	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;
	// --- End CommonActivatableWidget Interface

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnHealthChanged(EAttributeType Attribute);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLeveledUp(int PreviousLevel, int CurrentLevel, float NextLevelExp);

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UGameplaySystemComponent> GameplaySystem;

private:
	FDelegateHandle HealthHandle;
	FDelegateHandle MaxHealthHandle;
};
