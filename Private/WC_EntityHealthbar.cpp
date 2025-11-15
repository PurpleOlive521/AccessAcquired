// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "WC_EntityHealthbar.h"

#include "GameplaySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameFramework/Character.h"

void UWC_EntityHealthbar::BindToDelegates()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (IGameplaySystemOwnerInterface* GameplaySystemInterface = Cast<IGameplaySystemOwnerInterface>(Owner))
	{
		OwnerGameplaySystem = GameplaySystemInterface->Execute_GetGameplaySystemComponent(Owner);

		check(OwnerGameplaySystem);

		OwnerGameplaySystem->OnLeveledUpDelegate.AddUniqueDynamic(this, &UWC_EntityHealthbar::OnLeveledUp);

		OwnerGameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_Health).AddUniqueDynamic(this, &UWC_EntityHealthbar::OnHealthChanged);
		OwnerGameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_MaxHealth).AddUniqueDynamic(this, &UWC_EntityHealthbar::OnHealthChanged);

		OwnerGameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_Charge).AddUniqueDynamic(this, &UWC_EntityHealthbar::OnChargeChanged);
		OwnerGameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_MaxCharge).AddUniqueDynamic(this, &UWC_EntityHealthbar::OnChargeChanged);
	}
}

void UWC_EntityHealthbar::UnbindFromDelegates()
{
	if (OwnerGameplaySystem)
	{
		OwnerGameplaySystem->OnLeveledUpDelegate.RemoveDynamic(this, &UWC_EntityHealthbar::OnLeveledUp);

		OwnerGameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_Health).RemoveDynamic(this, &UWC_EntityHealthbar::OnHealthChanged);
		OwnerGameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_MaxHealth).RemoveDynamic(this, &UWC_EntityHealthbar::OnHealthChanged);

		OwnerGameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_Charge).RemoveDynamic(this, &UWC_EntityHealthbar::OnChargeChanged);
		OwnerGameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_MaxCharge).RemoveDynamic(this, &UWC_EntityHealthbar::OnChargeChanged);
	}
}