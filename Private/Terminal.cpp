// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "Terminal.h"

#include "Kismet/KismetSystemLibrary.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameplaySystemComponent.h"
#include "GameFramework/Character.h"

#include "DevCommons.h"

// Sets default values
ATerminal::ATerminal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATerminal::BeginPlay()
{
	Super::BeginPlay();

	TInlineComponentArray<USceneComponent*> SceneComponents(this, false);

	for (USceneComponent* SceneComp : SceneComponents)
	{
		if (SceneComp->GetName() == "WidgetLocation")
		{
			WidgetLocationComponent = SceneComp;
			break;
		}
	}

	if (!WidgetLocationComponent)
	{
		UE_LOG(VSLog, Error, TEXT("Could not find WidgetComponent named WidgetLocation!"))
	}
	
}

// Called every frame
void ATerminal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATerminal::Interact_Implementation(AActor* InInteractor)
{
	MoveInteractorToIdealLocation(InInteractor);
}

FInteractionParams ATerminal::GetInteractionData_Implementation()
{
	if (WidgetLocationComponent)
	{
		InteractionParams.InteractWidgetLocation = WidgetLocationComponent->GetComponentLocation();
	}
	return InteractionParams;
}

FVector ATerminal::GetIdealInteractorLocation() const
{
	const FVector RightOffset = GetActorRightVector() * 140.0f;
	const FVector ForwardOffset = GetActorForwardVector() * -25.0f;

	return 	GetActorLocation() + RightOffset + ForwardOffset;
}

void ATerminal::HealInteractor()
{
	if (IGameplaySystemOwnerInterface* GameplaySystemInterface = Cast<IGameplaySystemOwnerInterface>(Interactor))
	{
		UGameplaySystemComponent* GameplaySystem = GameplaySystemInterface->Execute_GetGameplaySystemComponent(Cast<UObject>(Interactor));

		const float MaxHealth = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxHealth);
		GameplaySystem->SetAttributeBaseValue(EAttributeType::EAT_Health, MaxHealth);
	}
}

void ATerminal::DrawIdealInteractorLocation() const
{
	UKismetSystemLibrary::DrawDebugSphere(this, GetIdealInteractorLocation(), 50, 12, FLinearColor::Yellow, 2.0f, 1.0f);
}

