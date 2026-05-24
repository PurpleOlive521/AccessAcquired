// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#include "GateControllerComponent.h"
#include "GateComponent.h"

UGateControllerComponent::UGateControllerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGateControllerComponent::OnRegister()
{
	Super::OnRegister();

	GatherGateComponents();
}

void UGateControllerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	GatherGateComponents();
}

bool UGateControllerComponent::OnSerialize(FSaveGameArchive& Archive, bool bIsLoading)
{
	 const bool bSuccess = Archive.SerializeField(TEXT("GateControllerComponent"), [&](FStructuredArchive::FSlot Slot)
		{
			SerializeScriptProperties(Slot);

			if (LOADING)
			{
				SetGateStates_Instantaneous(LeaderState);
			}
		});

	return bSuccess;
}

void UGateControllerComponent::OpenGates()
{
	SetLeaderState(EGateState::EGS_Open);

	SyncGates();
}

void UGateControllerComponent::CloseGates()
{
	SetLeaderState(EGateState::EGS_Closed);

	SyncGates();
}

void UGateControllerComponent::InvertGatesState()
{
	const EGateState NewState = LeaderState == EGateState::EGS_Closed ? EGateState::EGS_Open : EGateState::EGS_Closed;

	SetLeaderState(NewState);

	SyncGates();
}

void UGateControllerComponent::SetGateStates(EGateState InState)
{
	SetLeaderState(InState);
	
	SyncGates();
}

void UGateControllerComponent::SetGateStates_Instantaneous(EGateState InState)
{
	SetLeaderState(InState);

	for (auto Component : GateComponents)
	{
		Component->SetGateState_Instantaneous(LeaderState);
	}
}

inline void UGateControllerComponent::SyncGates()
{
	for (auto Component : GateComponents)
	{
		Component->SetGateState(LeaderState);
	}
}

void UGateControllerComponent::GatherGateComponents()
{
	if (AActor* Owner = GetOwner())
	{
		TInlineComponentArray<UGateComponent*> OutArray;
		Owner->GetComponents<UGateComponent>(OutArray);

		for (auto Component : OutArray)
		{
			GateComponents.Add(MakeWeakObjectPtr(Component));
		}
	}
}

inline void UGateControllerComponent::SetLeaderState(EGateState InState)
{
	if (LeaderState == InState)
	{
		return;
	}

	LeaderState = InState;

	OnGateStatesChangedDelegate.Broadcast(LeaderState);
}
