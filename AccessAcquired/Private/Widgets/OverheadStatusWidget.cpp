// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/OverheadStatusWidget.h"

#include "GameplaySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameFramework/Character.h"
#include "GameplayTagDefines.h"

void UOverheadStatusWidget::InitializeWithOwner(AActor* InOwner)
{
	check(InOwner);

	Owner = MakeWeakObjectPtr(InOwner);

	BindToOwner();

	K2_OnInitializedWithOwner();

	// Immediately trigger callbacks to sync UI state
	K2_OnHealthChanged(EAttributeType::EAT_Health);
	K2_OnChargeChanged(EAttributeType::EAT_Charge);
	K2_OnLeveledUp(0, OwnerGameplaySystem->GetEntityLevel(), 0.0f);
}

UGameplaySystemComponent* UOverheadStatusWidget::GetBoundGameplaySystem() const
{
	return OwnerGameplaySystem.Get();
}

AActor* UOverheadStatusWidget::GetBoundActor() const
{
	return Owner.Get();
}

void UOverheadStatusWidget::BindToOwner()
{
	AActor* DerefOwner = Owner.Get();

	UGameplaySystemComponent* RawGameplaySystem = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(DerefOwner);
	check(RawGameplaySystem);
	OwnerGameplaySystem = MakeWeakObjectPtr(RawGameplaySystem);

	RawGameplaySystem->OnLeveledUpDelegate.AddUniqueDynamic(this, &UOverheadStatusWidget::K2_OnLeveledUp);

	FGameplayTagSystem& TagSystem = RawGameplaySystem->GetGameplayTagSystemAsRef();
	TagSystem.OnGameplayTagChangedDelegate.AddUObject(this, &UOverheadStatusWidget::ListenForDeathTags);

	FDelegateCollection& Delegates = RawGameplaySystem->OnAttributeChangedDelegateCollection;

	HealthHandle = Delegates.GetDelegate(EAttributeType::EAT_Health).AddUObject(this, &UOverheadStatusWidget::K2_OnHealthChanged);
	MaxHealthHandle = Delegates.GetDelegate(EAttributeType::EAT_MaxHealth).AddUObject(this, &UOverheadStatusWidget::K2_OnHealthChanged);

	ChargeHandle = Delegates.GetDelegate(EAttributeType::EAT_Charge).AddUObject(this, &UOverheadStatusWidget::K2_OnChargeChanged);
	MaxChargeHandle = Delegates.GetDelegate(EAttributeType::EAT_MaxCharge).AddUObject(this, &UOverheadStatusWidget::K2_OnChargeChanged);
}

void UOverheadStatusWidget::UnbindFromOwner()
{
	if (UGameplaySystemComponent* DerefGameplaySystem = OwnerGameplaySystem.Get())
	{
		DerefGameplaySystem->OnLeveledUpDelegate.RemoveDynamic(this, &UOverheadStatusWidget::K2_OnLeveledUp);

		FGameplayTagSystem& TagSystem = DerefGameplaySystem->GetGameplayTagSystemAsRef();
		TagSystem.OnGameplayTagChangedDelegate.RemoveAll(this);

		FDelegateCollection& Delegates = DerefGameplaySystem->OnAttributeChangedDelegateCollection;

		Delegates.GetDelegate(EAttributeType::EAT_Health).Remove(HealthHandle);
		Delegates.GetDelegate(EAttributeType::EAT_MaxHealth).Remove(MaxHealthHandle);

		Delegates.GetDelegate(EAttributeType::EAT_Charge).Remove(ChargeHandle);
		Delegates.GetDelegate(EAttributeType::EAT_MaxCharge).Remove(MaxChargeHandle);
	}
}

void UOverheadStatusWidget::ListenForDeathTags(FGameplayTag ModifiedTag, bool bWasAdded)
{
	if (!bWasAdded)
	{
		return;
	}

	if (ModifiedTag == GAMEPLAYTAG_Status_Dying || ModifiedTag == GAMEPLAYTAG_Status_Dead)
	{
		K2_OnOwnerDead();
		UnbindFromOwner();
		OwnerGameplaySystem = nullptr;
		Owner = nullptr;
	}
}