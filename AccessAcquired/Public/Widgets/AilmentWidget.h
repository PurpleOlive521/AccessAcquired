// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffect.h"
#include "AilmentWidget.generated.h"

class UGameplaySystemComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UAilmentWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Initializes the widget with a Ailment to display
	UFUNCTION(BlueprintCallable)
	void InitializeWidget(UGameplaySystemComponent* GameplaySystem, FGameplayEffectHandle Handle);

	// Numerical changes, such as updating the StackProgress. Not for large asset changes, such as different textures or content.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On GameplayEffect Changed"), Category = "AilmentWidget")
	void K2_OnGameplayEffectChanged(const FActiveGameplayEffect& ActiveEffect, bool bIsAtFullStacks);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Receive GameplayEffect Asset"), Category = "AilmentWidget")
	void K2_ReceiveGameplayEffectAsset(UGameplayEffectVisualsAsset* VisualAsset);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Remove Widget"), Category = "AilmentWidget")
	void K2_RemoveWidget();

	// Called when the Ailment is about to run out.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Ailment Running Out"), Category = "AilmentWidget")
	void K2_OnAilmentRunningOut();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Ailment Stop Running Out"), Category = "AilmentWidget")
	void K2_OnAilmentStopRunningOut();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Stack Applied"), Category = "AilmentWidget")
	void K2_OnStackApplied();

	// Called before the Widget is removed.
	void OnGameplayEffectRemoved();

	void BindToGameplayEffect();

	void UnbindFromGameplayEffect();

	UFUNCTION()
	void OnGameplayEffectStacksChanged(int32 NewStackCount);

	UFUNCTION()
	void OnGameplayEffectStackProgressChanged(float NewStackProgress);

	FActiveGameplayEffect* GetActiveEffect() const;

	void SetIsRunningOut(bool bInState);

	UPROPERTY(BlueprintReadOnly, Category = "AilmentWidget")
	FGameplayEffectHandle TrackedHandle;

	UPROPERTY(EditAnywhere, Category = "AilmentWidget")
	float RunningOutStackProgressThreshold = 1.5f;

	UPROPERTY(EditAnywhere, Category = "AilmentWidget")
	float RunningOutActiveTimeThreshold = 1.0f;

	int32 LastStackCount = 0;

protected:

	TWeakObjectPtr<UGameplaySystemComponent> BoundGameplaySystem = nullptr;

	bool bIsRunningOut = false;
};
