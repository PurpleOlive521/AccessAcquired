// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "InteractionComponent.h"

#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "DevCommons.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameplaySystemComponent.h"


// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AActor* UInteractionComponent::GetActiveInteractable(TArray<AActor*> Interactables)
{
	if (Interactables.Num() == 0) 
	{
		return nullptr;
	}


	float ClosestToOne = 0;
	int BestIndex = 0;
	bool bFirst = true;

	FVector SelfLocation = GetOwner()->GetActorLocation();
	FVector SelfForward = GetOwner()->GetActorForwardVector();

	for (int i = 0; i < Interactables.Num(); i++)
	{
		FVector InteractableLocation = Interactables[i]->GetActorLocation();
		FVector DirectionToInteractable = InteractableLocation - SelfLocation;
		DirectionToInteractable.Normalize();
		
		float DotProduct = FMath::Abs(1 - FVector::DotProduct(DirectionToInteractable, SelfForward));
		float Weight = DotProduct;


		if (bFirst)
		{
			bFirst = false;
			BestIndex = i;
			ClosestToOne = Weight;
		}

		if (Weight < ClosestToOne)
		{
			BestIndex = i;
			ClosestToOne = Weight;
		}
	}

	return Interactables[BestIndex];
}

bool UInteractionComponent::CheckInteractionRequirements(FInteractionRequirements Requirements, bool bPressedPreviously)
{
	// Interaction impossible
	if (Requirements.bInteractionImpossible == true) 
	{
		return false;
	}

	// Button already pressed before
	if (Requirements.bCanOnlyBePressedOnce && bPressedPreviously == true) 
	{
		return false;
	}

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Player == nullptr)
	{
		UE_LOG(VSLog, Error, TEXT("InteractionComponent: Error - No PlayerCharacter found"));
		return false;
	}

	UGameplaySystemComponent* GameplaySystem = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(Player);
	check(GameplaySystem);

	if (GameplaySystem->GetGameplayTagSystem()->HasAllTags(Requirements.RequiredTags) == false)
	{
		return false;
	}

	// We do cast-specific checks last to avoid casting unless strictly necessary
	APlayerCharacter* CastedPlayer = Cast<APlayerCharacter>(Player);

	if (CastedPlayer == nullptr) 
	{
		UE_LOG(VSLog, Error, TEXT("InteractionComponent: Error - Could not cast PlayerCharacter"));
		return false;
	}

	if (Requirements.bAllowedDuringCombat == false && CastedPlayer->IsPlayerInCombat() == true)
	{
		return false;
	}

	return true;
}
