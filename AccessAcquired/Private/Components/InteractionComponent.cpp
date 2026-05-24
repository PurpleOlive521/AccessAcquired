// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "InteractionComponent.h"

#include "Kismet/GameplayStatics.h"
#include "DevCommons.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
		{
			Capsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &UInteractionComponent::OnReceiveBeginOverlap);
			Capsule->OnComponentEndOverlap.AddUniqueDynamic(this, &UInteractionComponent::OnReceiveEndOverlap);
		}
		
		CreateInteractionWidget();
	}
}


void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessInteractables();

	KeepWidgetOnInteractable(FocusInteractable);
}

bool UInteractionComponent::CheckInteractionRequirements(const FInteractionParams& Params)
{
	const FInteractionRequirements& Requirements = Params.InteractionRequirements;

	// Interaction impossible
	if (Requirements.bInteractionImpossible)
	{
		return false;
	}

	// Button already pressed before
	if (Requirements.bCanOnlyBePressedOnce && Params.bPreviouslyInteractedWith)
	{
		return false;
	}

	UGameplaySystemComponent* GameplaySystem = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(GetOwner());
	check(GameplaySystem);
	FGameplayTagSystem* TagSystem = GameplaySystem->GetGameplayTagSystem();

	if (not TagSystem->HasAllTags(Requirements.RequiredTags))
	{
		return false;
	}

	const bool bIsInCombat = TagSystem->HasTag(GAMEPLAYTAG_Status_CombatReady_InCombat);
	if (not Requirements.bAllowedDuringCombat && bIsInCombat)
	{
		return false;
	}

	return true;
}

void UInteractionComponent::InteractWithFocusInteractable()
{
	if (FocusInteractable)
	{
		FInteractionParams Params = IInteractableInterface::Execute_GetInteractionData(FocusInteractable);

		const bool bIsInteractable = CheckInteractionRequirements(Params);
		if (bIsInteractable)
		{
			IInteractableInterface::Execute_Interact(FocusInteractable, GetOwner());

			InteractionWidgetInstance->OnInteract();
		}
	}
}

void UInteractionComponent::OnReceiveBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UInteractableInterface>())
	{
		if (not AvailableInteractables.Contains(OtherActor))
		{
			AvailableInteractables.Add(OtherActor);

			ProcessInteractables();
		}
	}
}

void UInteractionComponent::OnReceiveEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->Implements<UInteractableInterface>())
	{
		AvailableInteractables.Remove(OtherActor);

		if (FocusInteractable == OtherActor)
		{
			ProcessInteractables();
		}
	}
}

void UInteractionComponent::AssignFocusInteractable(AActor* Interactable)
{
	if (Interactable && Interactable->Implements<UInteractableInterface>())
	{
		if (FocusInteractable != Interactable)
		{
			FocusInteractable = Interactable;

			const FInteractionParams Params = IInteractableInterface::Execute_GetInteractionData(FocusInteractable);

			InteractionWidgetInstance->SetContent(Params);
			InteractionWidgetInstance->OnSwitchedInteractable();

			KeepWidgetOnInteractable(FocusInteractable);
		}
	}
	else
	{
		if (FocusInteractable)
		{
			InteractionWidgetInstance->OnLeftInteractable();
		}

		FocusInteractable = nullptr;
	}
}

void UInteractionComponent::KeepWidgetOnInteractable(AActor* Interactable)
{
	if (InteractionWidgetInstance)
	{
		if (Interactable)
		{
			FInteractionParams Params = IInteractableInterface::Execute_GetInteractionData(Interactable);

			InteractionWidgetInstance->SetWorldLocation(Params.InteractWidgetLocation);
		}
	}
}

void UInteractionComponent::CreateInteractionWidget()
{
	if (not InteractionWidgetInstance)
	{
		UActorComponent* Component = GetOwner()->AddComponentByClass(InteractionWidgetClass, true /* bManualAttachment */, FTransform::Identity, false /* bDeferredFinish */);
		InteractionWidgetInstance = Cast<UWC_InteractionPrompt>(Component);
	}
}

void UInteractionComponent::ProcessInteractables()
{
	bool bRequiresCleanup = false;

	// Most implementing objects won't be destroyed during use, so we expect an stale pointer to be a rare occurance.
	TArray<TObjectPtr<AActor>> InvalidInteractables;

	const FVector SelfLocation = GetOwner()->GetActorLocation();
	const FVector SelfForward = GetOwner()->GetActorForwardVector();

	TObjectPtr<AActor> ChosenInteractable = nullptr;
	float BestWeight = FLT_MAX;

	// Pick the forward-most interactable that we can currently interact with.
	for (const auto& Interactable : AvailableInteractables)
	{
		if (not Interactable)
		{
			bRequiresCleanup = true;
			InvalidInteractables.Add(Interactable);
			continue;
		}

		FInteractionParams Params = IInteractableInterface::Execute_GetInteractionData(Interactable);

		const bool bIsInteractable = CheckInteractionRequirements(Params);
		if (bIsInteractable)
		{
			FVector DirectionToInteractable = Interactable->GetActorLocation() - SelfLocation;
			DirectionToInteractable.Normalize();

			float Weight = FMath::Abs(1 - FVector::DotProduct(DirectionToInteractable, SelfForward));

			if (Weight < BestWeight)
			{
				ChosenInteractable = Interactable;
				BestWeight = Weight;
			}
		}
	}

	AssignFocusInteractable(ChosenInteractable);
	
	if (bRequiresCleanup)
	{
		for (const auto& InvalidInteractable : InvalidInteractables)
		{
			AvailableInteractables.Remove(InvalidInteractable);
		}
	}
}
