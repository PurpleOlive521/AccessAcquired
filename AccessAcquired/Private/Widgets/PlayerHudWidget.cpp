// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "PlayerHudWidget.h"

#include "GameplaySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameFramework/Character.h"

void UPlayerHudWidget::NativeOnActivated()
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if(GameplaySystem = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(PlayerCharacter))
	{
		GameplaySystem->OnLeveledUpDelegate.AddUniqueDynamic(this, &UPlayerHudWidget::OnLeveledUp);

		FDelegateCollection& Delegates = GameplaySystem->OnAttributeChangedDelegateCollection;

		HealthHandle = Delegates.GetDelegate(EAttributeType::EAT_Health).AddUObject(this, &UPlayerHudWidget::OnHealthChanged);
		MaxHealthHandle = Delegates.GetDelegate(EAttributeType::EAT_MaxHealth).AddUObject(this, &UPlayerHudWidget::OnHealthChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerHudWidget::NativeOnActivated - Could not find GameplaySystemComponent on PlayerCharacter"));
	}



	// Blueprint relies on us caching the GameplaySystem first
	Super::NativeOnActivated();
}

void UPlayerHudWidget::NativeOnDeactivated()
{
	// Deactivation can trigger on PIE or game close, first check if it's valid
	if (GameplaySystem)
	{
		GameplaySystem->OnLeveledUpDelegate.RemoveDynamic(this, &UPlayerHudWidget::OnLeveledUp);

		FDelegateCollection& Delegates = GameplaySystem->OnAttributeChangedDelegateCollection;

		Delegates.GetDelegate(EAttributeType::EAT_Health).Remove(HealthHandle);
		Delegates.GetDelegate(EAttributeType::EAT_MaxHealth).Remove(MaxHealthHandle);
	}

	Super::NativeOnDeactivated();
}



