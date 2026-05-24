// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffect.h"
#include "AilmentListWidget.generated.h"

class UAilmentWidget;
class UGameplaySystemComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UAilmentListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	// Initializes the widget with a GameplaySystem to track and display GameplayEffects for.
	UFUNCTION(BlueprintCallable, Category = "AilmentListWidget")
	void InitializeWidget(UGameplaySystemComponent* GameplaySystem);

	UFUNCTION()
	void ListenForGameplayEffectAdded(TSubclassOf<UGameplayEffect> EffectClass, const FGameplayEffectHandle& AddedHandle);

	UFUNCTION()
	void ListenForGameplayEffectRemoved(const FGameplayEffectHandle& RemovedHandle);

	void BindToGameplaySystem();

	void UnbindFromGameplaySystem();

	void AddAilmentWidget(TSubclassOf<UGameplayEffect> EffectClass, const FGameplayEffectHandle& AddedHandle);

	bool ShouldDisplayGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Create Ailment Widget"), Category = "AilmentListWidget")
	UAilmentWidget* K2_CreateWidget();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Add Ailment Widget"), Category = "AilmentListWidget")
	void K2_AddAilmentWidget(UAilmentWidget* Widget);

	void RemoveAilmentWidget(TSubclassOf<UGameplayEffect> EffectClass);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On All Ailment Widgets Removed"), Category = "AilmentListWidget")
	void K2_OnAllAilmentWidgetsRemoved();

protected:

	// The GameplayEffects that can be displayed in the widget.
	UPROPERTY(EditAnywhere, Category = "AilmentListWidget")
	TArray<TSubclassOf<UGameplayEffect>> DisplayedGameplayEffects;

	TMap<TSubclassOf<UGameplayEffect>, TObjectPtr<UAilmentWidget>> GameplayEffectWidgets;

	TWeakObjectPtr<UGameplaySystemComponent> BoundGameplaySystem = nullptr;
};
