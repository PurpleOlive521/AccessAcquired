// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "DamagePopContainerWidget.h"

#include "Kismet/GameplayStatics.h"
#include "GameplayMessageTypes.h"
#include "GameplayTagMessageDefines.h"
#include "DamagePopWidget.h"
#include "DevCommons.h"
#include "Components/CanvasPanel.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"


void UDamagePopContainerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	UGameplayMessageSubsystem* MessageSubsystem = UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(GameInstance);
	if (MessageSubsystem)
	{
		FGameplayMessageListenerParams<FAADamageEventMessage> Params;

		// TODO: Invert Channel tag order to make this partial, so we can pick up more hit messages
		Params.MatchType = EGameplayMessageMatch::ExactMatch;
		Params.SetMessageReceivedCallback(this, &UDamagePopContainerWidget::ReceiveDamageEvent);

		MessageHandle = MessageSubsystem->RegisterListener<FAADamageEventMessage>(GAMEPLAYTAG_Channels_Combat_DamageEvents, Params);
	}

	Setup();
}

void UDamagePopContainerWidget::NativeDestruct()
{
	MessageHandle.Unregister();

	Super::NativeDestruct();
}

void UDamagePopContainerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UDamagePopContainerWidget::Setup()
{
	for (int i = 0; i < WidgetPoolSize; i++)
	{
		CreateWidget();
	}
}

void UDamagePopContainerWidget::ReceiveDamageEvent(FGameplayTag Channel, const FAADamageEventMessage& EventMessage)
{
	FAAResolvedDamageEvent DamageEvent = EventMessage.ResolvedDamageEvent;
	if (DamageEvent.EventStates.bShowDamageNumbers)
	{
		if (DamageEvent.Target != UGameplayStatics::GetPlayerCharacter(this, 0))
		{
			UDamagePopWidget* PopupWidget = GetAvailableWidget();

			// Move it with a offset so the widgets don't overlap in world-space
			if (HasActiveWidgetCloseToLocation(DamageEvent.HitLocation))
			{
				DamageEvent.HitLocation += GetRandomOffset();
			}

			PopupWidget->SetAndPlay(DamageEvent, K2_GetRootWidget());
		}
	}
}

UDamagePopWidget* UDamagePopContainerWidget::GetAvailableWidget()
{
	if (AvailableWidgets.IsEmpty())
	{
		UE_LOG(VSLog, Warning, TEXT("DamagePopupContainerWidget: All pre-allocated widgets are used. Consider increasing WidgetPoolSize."));

		UDamagePopWidget* NewWidget = CreateAndConsumeWidget();
		return NewWidget;
	}

	const int WidgetIndex = AvailableWidgets.Pop();

	return WidgetPool[WidgetIndex];
}

UDamagePopWidget* UDamagePopContainerWidget::CreateWidget()
{
	UDamagePopWidget* NewWidget = NewObject<UDamagePopWidget>(this, WidgetClass);
	NewWidget->Setup(this);

	int32 Index = WidgetPool.Add(NewWidget);

	AvailableWidgets.Push(Index);

	return NewWidget;
}

UDamagePopWidget* UDamagePopContainerWidget::CreateAndConsumeWidget()
{
	UDamagePopWidget* NewWidget = CreateWidget();

	AvailableWidgets.Pop();
	
	return NewWidget;
}

void UDamagePopContainerWidget::ReturnWidget(UDamagePopWidget* Widget)
{
	int32 WidgetIndex = WidgetPool.IndexOfByKey(Widget);

	if (WidgetIndex == INDEX_NONE)
	{
		UE_LOG(VSLog, Log, TEXT("DamagePopupContainerWidget: Tried to return a widget that does not belong to this container."));
		return;
	}

	// Drop the incoming widget if we are above the pool size + buffer.
	if (WidgetPool.Num() > WidgetPoolSize + DECAY_SIZE_BUFFER)
	{
		WidgetPool.RemoveAt(WidgetIndex);

		return;
	}

	AvailableWidgets.Push(WidgetIndex);

}

bool UDamagePopContainerWidget::HasActiveWidgetCloseToLocation(FVector WorldLocation)
{
	for (const auto& Widget : WidgetPool)
	{
		if (Widget)
		{
			if (Widget->IsActive())
			{
				if ((WorldLocation - Widget->GetWorldLocation()).Length() <= DistanceRandomisationThreshold)
				{
					return true;
				}
			}
		}
	}

	return false;
}

FVector UDamagePopContainerWidget::GetRandomOffset() const
{
	return UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(RandomisedOffsetDistance / 2.0f, RandomisedOffsetDistance);
}
