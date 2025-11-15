// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

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

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnEnergyChanged(EAttributeType Attribute);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLeveledUp(int PreviousLevel, int CurrentLevel, float NextLevelExp);




protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UGameplaySystemComponent> GameplaySystem;
};
