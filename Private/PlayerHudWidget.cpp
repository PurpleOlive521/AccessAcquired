// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "PlayerHudWidget.h"

#include "GameplaySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameFramework/Character.h"

void UPlayerHudWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if(IGameplaySystemOwnerInterface* GameplaySystemInterface = Cast<IGameplaySystemOwnerInterface>(PlayerCharacter))
	{
		GameplaySystem = GameplaySystemInterface->Execute_GetGameplaySystemComponent(PlayerCharacter);

		check(GameplaySystem);

		GameplaySystem->OnLeveledUpDelegate.AddUniqueDynamic(this, &UPlayerHudWidget::OnLeveledUp);

		GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_Health).AddUniqueDynamic(this, &UPlayerHudWidget::OnHealthChanged);
		GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_MaxHealth).AddUniqueDynamic(this, &UPlayerHudWidget::OnHealthChanged);

		GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_Charge).AddUniqueDynamic(this, &UPlayerHudWidget::OnEnergyChanged);
		GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_MaxCharge).AddUniqueDynamic(this, &UPlayerHudWidget::OnEnergyChanged);
	}
}

void UPlayerHudWidget::NativeOnDeactivated()
{
	// Deactivation can trigger on PIE or game close, first check if it's valid
	if (GameplaySystem)
	{
		GameplaySystem->OnLeveledUpDelegate.RemoveDynamic(this, &UPlayerHudWidget::OnLeveledUp);

		GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_Health).RemoveDynamic(this, &UPlayerHudWidget::OnHealthChanged);
		GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_MaxHealth).RemoveDynamic(this, &UPlayerHudWidget::OnHealthChanged);

		GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_Energy).RemoveDynamic(this, &UPlayerHudWidget::OnEnergyChanged);
		GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_MaxEnergy).RemoveDynamic(this, &UPlayerHudWidget::OnEnergyChanged);
	}

	Super::NativeOnDeactivated();
}



