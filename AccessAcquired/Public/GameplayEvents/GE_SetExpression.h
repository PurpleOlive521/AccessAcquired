// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEvents/GameplayEvent.h"
#include "GE_SetExpression.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UGE_SetExpression : public UGameplayEvent
{
	GENERATED_BODY()
	
public:

	UGE_SetExpression();

	// Sets the ExpressionMaterial material on the owning Actor's mesh at the specified Material Index.
	UFUNCTION(BlueprintCallable)
	void SetExpression();

	// Sets the ExpressionMaterial back to it's pre-event material.
	UFUNCTION(BlueprintCallable)
	void RestoreExpression();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GE_SetExpression")
	TObjectPtr<UMaterialInterface> ExpressionMaterial = nullptr;

	// The material slot index where the ExpressionMaterial material should be set.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GE_SetExpression")
	int MaterialIndex = 0;

	// Reverts the expression to it's previous material when the GameplayEvent ends.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GE_SetExpression")
	bool bRevertExpression = false;
	
protected:

	virtual void TriggerEvent(const FGameplayEventActivationData& ActivationData) override;

	virtual void EndEvent() override;

	virtual void AbortEvent() override;

	UPROPERTY(BlueprintReadOnly, Category = "GE_SetExpression")
	TObjectPtr<UMaterialInterface> OriginalMaterial = nullptr;
};
