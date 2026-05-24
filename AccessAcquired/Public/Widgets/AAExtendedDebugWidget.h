// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplaySystemDebugWidget.h"
#include "AAExtendedDebugWidget.generated.h"

class AEnemyAIControllerBase;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UAAExtendedDebugWidget : public UGameplaySystemDebugWidget
{
	GENERATED_BODY()
	
public:

	void OnEnabledTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void OnBoundToGameplaySystem(UGameplaySystemComponent* GameplaySystem) override;

	// Only called when it's safe to update the AI Controller display.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Tick AI Controller Display"), Category = "DebugWidget")
	void K2_TickAIControllerDisplay(float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AAExtendedDebugWidget")
	FString GetAIControllerGenericDisplayInfo() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AAExtendedDebugWidget")
	FString GetAIControllerTargetDisplayInfo() const;

protected:

	void BindToAIController();

	TWeakObjectPtr<AEnemyAIControllerBase> BoundAIController = nullptr;
};
