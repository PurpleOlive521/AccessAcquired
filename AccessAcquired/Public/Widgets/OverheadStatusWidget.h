// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "AttributeTypes.h"
#include "OverheadStatusWidget.generated.h"

class UGameplaySystemComponent;

/**
 * Widget that binds to an Actor and displays key gameplay information such as Health and Level.
 */
UCLASS()
class VERTICALSLICE_API UOverheadStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	// Initializes the widget with the Actor to bind to.
	UFUNCTION(BlueprintCallable)
	void InitializeWithOwner(AActor* InOwner);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Initialized With Owner"))
	void K2_OnInitializedWithOwner();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGameplaySystemComponent* GetBoundGameplaySystem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetBoundActor() const;
	
	void BindToOwner();

	UFUNCTION(BlueprintCallable)
	void UnbindFromOwner();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Level Changed"))
	void K2_OnLevelChanged(int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Health Changed"))
	void K2_OnHealthChanged(EAttributeType Attribute);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Charge Changed"))
	void K2_OnChargeChanged(EAttributeType Attribute);

	void ListenForDeathTags(FGameplayTag ModifiedTag, bool bWasAdded);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Owner Dead"))
	void K2_OnOwnerDead();

protected:
	TWeakObjectPtr<UGameplaySystemComponent> OwnerGameplaySystem = nullptr;

	TWeakObjectPtr<AActor> Owner = nullptr;

private:
	FDelegateHandle HealthHandle;
	FDelegateHandle MaxHealthHandle;
	FDelegateHandle ChargeHandle;
	FDelegateHandle MaxChargeHandle;
};
