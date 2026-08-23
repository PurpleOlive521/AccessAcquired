// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoreStatsWidget.generated.h"

class UGameplaySystemComponent;

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UCoreStatsWidget : public UUserWidget
{
	GENERATED_BODY()	

public:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void OnContentChanged();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Content Changed"), Category = "CoreStatsWidget")
	void K2_OnContentChanged(int32 Level, float ExperienceRequired, float CurrentExperience);

	UFUNCTION()
	void OnLevelChanged(int32 NewLevel);

	UFUNCTION()
	void OnExperienceChanged(float NewExperience);

	void BindToPlayer();

	void UnbindFromPlayer();

protected:

	TWeakObjectPtr<UGameplaySystemComponent> PlayerGameplaySystem = nullptr;
};
