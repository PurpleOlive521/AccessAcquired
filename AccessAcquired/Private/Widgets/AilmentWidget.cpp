// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/AilmentWidget.h"
#include "GameplaySystemComponent.h"
#include "AilmentListWidget.h"

void UAilmentWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAilmentWidget::NativeDestruct()
{
	UnbindFromGameplayEffect();

	Super::NativeDestruct();
}

void UAilmentWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (UGameplaySystemComponent* GameplaySystem = BoundGameplaySystem.Get())
	{
		if (FActiveGameplayEffect* ActiveEffect = GameplaySystem->GetGameplayEffectByHandle(TrackedHandle))
		{
			// Two conditions can apply to the IsRunningOut state: The StackProgress about to be removed due to inactivity in applying more, and the effect being active and about to be removed.
			if (ActiveEffect->IsStackProgressDurationInControl() && ActiveEffect->GetRemainingStackProgressDuration() <= RunningOutStackProgressThreshold)
			{
				SetIsRunningOut(true);
				return;
			}


			if (ActiveEffect->IsAtFullStacks() && ActiveEffect->GetRemainingDuration() <= RunningOutActiveTimeThreshold)
			{
				SetIsRunningOut(true);
				return;
			}

			SetIsRunningOut(false);
		}
	}
}

void UAilmentWidget::InitializeWidget(UGameplaySystemComponent* GameplaySystem, FGameplayEffectHandle Handle)
{
	check(GameplaySystem);

	BoundGameplaySystem = MakeWeakObjectPtr(GameplaySystem);

	TrackedHandle = Handle;

	BindToGameplayEffect();

	if (FActiveGameplayEffect* ActiveEffect = GetActiveEffect())
	{
		const UGameplayEffect* Definition = ActiveEffect->GetDefinition();

		if (UGameplayEffectVisualsAsset* Asset = Definition->VisualsAsset)
		{
			K2_ReceiveGameplayEffectAsset(Asset);
		}
	}
}

void UAilmentWidget::OnGameplayEffectRemoved()
{
	UnbindFromGameplayEffect();

	K2_RemoveWidget();
}

void UAilmentWidget::BindToGameplayEffect()
{
	if (FActiveGameplayEffect* ActiveEffect = GetActiveEffect())
	{
		ActiveEffect->OnStackChangedSignature.AddUObject(this, &UAilmentWidget::OnGameplayEffectStacksChanged);
		ActiveEffect->OnStackProgressChangedSignature.AddUObject(this, &UAilmentWidget::OnGameplayEffectStackProgressChanged);
	}
}

void UAilmentWidget::UnbindFromGameplayEffect()
{
	if (FActiveGameplayEffect* ActiveEffect = GetActiveEffect())
	{
		ActiveEffect->OnStackChangedSignature.RemoveAll(this);
		ActiveEffect->OnStackProgressChangedSignature.RemoveAll(this);
	}
}

void UAilmentWidget::OnGameplayEffectStacksChanged(int32 NewStackCount)
{
	if (FActiveGameplayEffect* ActiveEffect = GetActiveEffect())
	{
		const bool bIsAtFullStacks = ActiveEffect->IsAtFullStacks();
		K2_OnGameplayEffectChanged(*ActiveEffect, bIsAtFullStacks);

		if (NewStackCount - LastStackCount > 0)
		{
			K2_OnStackApplied();
		}

		LastStackCount = NewStackCount;
	}
}

void UAilmentWidget::OnGameplayEffectStackProgressChanged(float NewStackProgress)
{
	if (FActiveGameplayEffect* ActiveEffect = GetActiveEffect())
	{
		const bool bIsAtFullStacks = ActiveEffect->IsAtFullStacks();
		K2_OnGameplayEffectChanged(*ActiveEffect, bIsAtFullStacks);
	}
}

FActiveGameplayEffect* UAilmentWidget::GetActiveEffect() const
{
	if (UGameplaySystemComponent* GameplaySystem = BoundGameplaySystem.Get())
	{
		return GameplaySystem->GetGameplayEffectByHandle(TrackedHandle);
	}		 

	return nullptr;
}

void UAilmentWidget::SetIsRunningOut(bool bInState)
{
	if (bIsRunningOut == bInState)
	{
		return;
	}

	bIsRunningOut = bInState;

	if (bIsRunningOut)
	{
		K2_OnAilmentRunningOut();
	}
	else
	{
		K2_OnAilmentStopRunningOut();
	}
}
