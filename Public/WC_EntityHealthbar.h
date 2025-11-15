// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "WC_EntityHealthbar.generated.h"

class UGameplaySystemComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UWC_EntityHealthbar : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void BindToDelegates();

	UFUNCTION(BlueprintCallable)
	void UnbindFromDelegates();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnLeveledUp(int PreviousLevel, int CurrentLevel, float NextLevelExp);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnHealthChanged(EAttributeType Attribute);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnChargeChanged(EAttributeType Attribute);

protected:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UGameplaySystemComponent> OwnerGameplaySystem;

};
