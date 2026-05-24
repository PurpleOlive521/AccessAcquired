// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AAHUD.h"
#include "Blueprint/UserWidget.h"
#include "WidgetMenuStack.h"
#include "Kismet/GameplayStatics.h"
#include "DevCommons.h"
#include "ConfirmationMenuWidget.h"

void AAAHUD::BeginPlay()
{
	Super::BeginPlay();
}

AAAHUD* AAAHUD::Get(const UObject* WorldContext)
{
	if (!WorldContext)
	{
		UE_LOG(VSLog, Error, TEXT("AAAHUD: Tried to get AAHUD with null WorldContext!"));
		return nullptr;
	}

	APlayerController* Controller = UGameplayStatics::GetPlayerController(WorldContext, 0);
	if (!Controller)
	{
		UE_LOG(VSLog, Error, TEXT("AAAHUD: Could not find PlayerController!"));
		return nullptr;
	}

	return Controller->GetHUD<AAAHUD>();
}

UWidgetMenuStack* AAAHUD::GetMenuStack(const UObject* WorldContext)
{
	AAAHUD* Hud = Get(WorldContext);

	return Hud->GetOrCreateMenuStack(WorldContext);
}

UWidgetMenuStack* AAAHUD::GetOrCreateMenuStack(const UObject* WorldContext)
{
	if (not MenuStackWidget)
	{
		APlayerController* Controller = UGameplayStatics::GetPlayerController(WorldContext, 0);

		UClass* MenuStackClass = DefaultWidgetMenuStackClass.LoadSynchronous();
		MenuStackWidget = CreateWidget<UWidgetMenuStack>(Controller, MenuStackClass);
		MenuStackWidget->AddToViewport(MENU_STACK_ZORDER);
	}

	return MenuStackWidget;
}

TSubclassOf<UConfirmationMenuWidget> AAAHUD::GetConfirmationWidgetClass() const
{
	return DefaultConfirmationWidgetClass.LoadSynchronous();
}
