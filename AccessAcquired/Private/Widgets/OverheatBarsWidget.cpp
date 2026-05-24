// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/OverheatBarsWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameplaySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameplayTagMessageDefines.h"
#include "DevelopmentTypes.h"

void UOverheatBarsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		if (UGameplaySystemComponent* GameplaySystem = UGameplaySystemComponent::GetGameplaySystemFromActor(Player))
		{
			TArray<FOnAttributeChangedSignature*> OutDelegates;
			GameplaySystem->OnAttributeChangedDelegateCollection.GetMultipleDelegates(AttributeGroups::OverheatAttributes, OutDelegates);
			for (auto& Delegate : OutDelegates)
			{
				Delegate->AddUObject(this, &UOverheatBarsWidget::ListenForOverheat);
			}

			BoundGameplaySystem = MakeWeakObjectPtr(GameplaySystem);
		}
	}

	if (UGameplayMessageSubsystem::HasInstance(this))
	{
		UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(this);

		FGameplayMessageListenerParams<FAAVerbMessage> Params;
		Params.MatchType = EGameplayMessageMatch::ExactMatch;
		Params.SetMessageReceivedCallback(this, &UOverheatBarsWidget::ReceiveMessage);

		MessageHandle = MessageSystem->RegisterListener(GAMEPLAYTAG_Channels_Combat_Player, Params);
	}
}

void UOverheatBarsWidget::NativeDestruct()
{
	MessageHandle.Unregister();

	if (UGameplaySystemComponent* GameplaySystemPtr = BoundGameplaySystem.Get())
	{
		TArray<FOnAttributeChangedSignature*> OutDelegates;
		GameplaySystemPtr->OnAttributeChangedDelegateCollection.GetMultipleDelegates(AttributeGroups::OverheatAttributes, OutDelegates);
		for (auto& Delegate : OutDelegates)
		{
			Delegate->RemoveAll(this);
		}
	}

	Super::NativeDestruct();
}

void UOverheatBarsWidget::ListenForOverheat(EAttributeType Attribute)
{
	if (UGameplaySystemComponent* GameplaySystemPtr = BoundGameplaySystem.Get())
	{
		const float Value = GameplaySystemPtr->GetAttributeValue(Attribute, EAttributeValue::EAV_CurrentValue);
		const float Max = GameplaySystemPtr->GetAttributeValue(EAttributeType::EAT_OverheatLimit, EAttributeValue::EAV_CurrentValue);
		const float Percentage = 100.0f * Value / Max;

		if (Percentage >= OverheatVisiblityThreshold)
		{
			switch (Attribute)
			{
				case EAttributeType::EAT_InfraredOverheat:
				{
					if (not bInfraredOverThreshold)
					{
						bInfraredOverThreshold = true;
						ModifyVisibilityCount(1);
					}
					break;
				}
				case EAttributeType::EAT_UltravioletOverheat:
				{
					if (not bUltravioletOverThreshold)
					{
						bUltravioletOverThreshold = true;
						ModifyVisibilityCount(1);
					}
					break;
				}
				case EAttributeType::EAT_GammaOverheat:
				{
					if (not bGammaOverThreshold)
					{
						bGammaOverThreshold = true;
						ModifyVisibilityCount(1);
					}
					break;
				}
				default:
				{
					ensureNoEntry(); // We bound to a non-overheat attribute?
				}
			}
		}
		else
		{
			switch (Attribute)
			{
				case EAttributeType::EAT_InfraredOverheat:
				{
					if (bInfraredOverThreshold)
					{
						bInfraredOverThreshold = false;
						ModifyVisibilityCount(-1);
					}
					break;
				}
				case EAttributeType::EAT_UltravioletOverheat:
				{
					if (bUltravioletOverThreshold)
					{
						bUltravioletOverThreshold = false;
						ModifyVisibilityCount(-1);
					}
					break;
				}
				case EAttributeType::EAT_GammaOverheat:
				{
					if (bGammaOverThreshold)
					{
						bGammaOverThreshold = false;
						ModifyVisibilityCount(-1);
					}
					break;
				}
				default:
				{
					ensureNoEntry(); // We bound to a non-overheat attribute?
				}
			}
		}
	}
}

void UOverheatBarsWidget::ReceiveMessage(FGameplayTag Channel, const FAAVerbMessage& Message)
{
	if (Message.Verb == GAMEPLAYTAG_Verbs_EnteredCombat)
	{
		ModifyVisibilityCount(1);
	}
	else if(Message.Verb == GAMEPLAYTAG_Verbs_ExitedCombat)
	{
		ModifyVisibilityCount(-1);
	}
}

void UOverheatBarsWidget::ModifyVisibilityCount(int32 Delta)
{
	VisiblityCount += Delta;

	OnVisibilityChanged();
}

void UOverheatBarsWidget::ForceWidgetVisibility(bool bVisible)
{
	if (bVisible)
	{
		SetForcedVisibility(EForcedVisibility::Visible);
	}
	else
	{
		SetForcedVisibility(EForcedVisibility::Hidden);
	}
}

void UOverheatBarsWidget::ClearForcedVisibility()
{
	SetForcedVisibility(EForcedVisibility::None);
}

void UOverheatBarsWidget::SetIsMovedAside(bool bInIsMovedAside)
{
	if (bIsMovedAside == bInIsMovedAside)
	{
		return;
	}

	bIsMovedAside = bInIsMovedAside;

	if (bIsMovedAside)
	{
		K2_MoveAsideWidget();
	}
	else
	{
		K2_MoveBackWidget();
	}
}

void UOverheatBarsWidget::OnVisibilityChanged()
{
	switch (ForcedVisibility)
	{
		case(EForcedVisibility::Visible):
		{
			SetCompoundVisibility(true /* bShouldBeVisible */);
			break;
		}

		case(EForcedVisibility::Hidden):
		{
			SetCompoundVisibility(false /* bShouldBeVisible */);
			break;
		}

		case(EForcedVisibility::None):
		{
			if (VisiblityCount > 0)
			{
				SetCompoundVisibility(true /* bShouldBeVisible */);
			}
			else
			{
				SetCompoundVisibility(false /* bShouldBeVisible */);
			}
			break;
		}
	}
}

void UOverheatBarsWidget::SetForcedVisibility(EForcedVisibility State)
{
	if (ForcedVisibility != State)
	{
		ForcedVisibility = State;
		OnVisibilityChanged();
	}
}

void UOverheatBarsWidget::SetCompoundVisibility(bool bShouldBeVisible)
{
	if (bShouldBeVisible)
	{
		if (not bCompoundIsVisible)
		{
			bCompoundIsVisible = true;
			K2_ShowWidget();
		}
	}
	else
	{
		if (bCompoundIsVisible)
		{
			bCompoundIsVisible = false;
			K2_HideWidget();
		}
	}
}
