// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "TargetLockableComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTagMessageDefines.h"
#include "GameplayMessageTypes.h"
#include "MeshAccessorInterface.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"

UTargetLockableComponent::UTargetLockableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTargetLockableComponent::BeginPlay()
{
	Super::BeginPlay();	

	AActor* Owner = GetOwner();
	SkeletalMesh = IMeshAccessorInterface::Execute_GetSkeletalMesh(Owner);
}

void UTargetLockableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindFromOwner();
	RequestStopLock();

	Super::EndPlay(EndPlayReason);
}

void UTargetLockableComponent::BindToOwner()
{
	if (UGameplaySystemComponent* RawGameplaySystem = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(GetOwner()))
	{
		GameplaySystem = MakeWeakObjectPtr(RawGameplaySystem);

		FGameplayTagSystem& TagSystem = RawGameplaySystem->GetGameplayTagSystemAsRef();
		TagSystem.OnGameplayTagChangedDelegate.AddUObject(this, &UTargetLockableComponent::ListenForDeathTags);

	}
}

void UTargetLockableComponent::UnbindFromOwner()
{
	if (UGameplaySystemComponent* DerefGameplaySystem = GameplaySystem.Get())
	{
		FGameplayTagSystem& TagSystem = DerefGameplaySystem->GetGameplayTagSystemAsRef();
		TagSystem.OnGameplayTagChangedDelegate.RemoveAll(this);
	}
}

void UTargetLockableComponent::RequestStopLock()
{
	// We might be called as due to PIE closing
	if (!GetWorld())
	{
		return;
	}

	FAAVerbMessage StopLockingMsg;
	StopLockingMsg.Instigator = this;
	StopLockingMsg.Verb = GAMEPLAYTAG_Verbs_StopTargetLock;

	if (UGameplayMessageSubsystem::HasInstance(this))
	{
		UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(this);
		MessageSystem->BroadcastMessage(GAMEPLAYTAG_Channels_GameplayEvents, StopLockingMsg);
	}
}

void UTargetLockableComponent::SetIsLockable(bool InLockable)
{
	if (bIsLockable == InLockable)
	{
		return;
	}

	bIsLockable = InLockable;

	// Something might be locked onto us when we switch
	if (bIsLockable)
	{
		RequestStopLock();
	}
}

bool UTargetLockableComponent::IsLockable() const
{
	return bIsLockable;
}

void UTargetLockableComponent::SetIsLockableInCombat(bool InLockableInCombat)
{
	bLockableInCombat = InLockableInCombat;
}

bool UTargetLockableComponent::IsLockableInCombat() const
{
	return bLockableInCombat;
}

FVector UTargetLockableComponent::GetDesiredIconLocation() const
{
	if (SkeletalMesh)
	{
		return SkeletalMesh->GetSocketLocation(SocketToTarget);
	}

	return GetOwner()->GetActorLocation();
}

void UTargetLockableComponent::ListenForDeathTags(FGameplayTag ModifiedTag, bool bWasAdded)
{
	if (!bWasAdded)
	{
		return;
	}

	if (ModifiedTag == GAMEPLAYTAG_Status_Dying || ModifiedTag == GAMEPLAYTAG_Status_Dead)
	{
		UnbindFromOwner();
		RequestStopLock();
	}
}

