// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "CharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "DevCommons.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	GameplaySystem = FindComponentByClass<UGameplaySystemComponent>();

	if (!GameplaySystem)
	{
		UE_LOG(VSLog, Error, TEXT("BaseCharacter: Could not find GameplaySystemComponent"));
	}

	CharacterMovement = FindComponentByClass<UCharacterMovementComponent>();

	if (!CharacterMovement)
	{
		UE_LOG(VSLog, Error, TEXT("BaseCharacter: Could not find CharacterMovementComponent"));
	}

	Super::BeginPlay();

	// Call last to ensure that all components are grabbed before calling delegates, since bound functions might require those components
	if (GameplaySystem)
	{
		MapMovementValues(EAttributeType::EAT_MovementSpeed);
		GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_MovementSpeed).AddUniqueDynamic(this, &ACharacterBase::MapMovementValues);
	}

}

void ACharacterBase::MapMovementValues(EAttributeType ChangedAttribute)
{
	float MovementSpeed = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MovementSpeed);

	// Scalars to keep acceleration and friction consistent to the new speed, ensuring a consistent game-feel as MovementSpeed increases or decreases
	float AccelerationScalar = MaxAcceleration / MaxWalkingSpeed;
	float FrictionScalar = GroundFriction / MaxWalkingSpeed;

	CharacterMovement->MaxWalkSpeed = MovementSpeed; //For AI movement
	CharacterMovement->MaxAcceleration = MovementSpeed * AccelerationScalar;
	CharacterMovement->GroundFriction = MovementSpeed * FrictionScalar;
}

UGameplaySystemComponent* ACharacterBase::GetGameplaySystemComponent_Implementation()
{
	if (!GameplaySystem)
	{
		UE_LOG(VSLog, Warning, TEXT("BaseCharacter: No GameplaySystem, attempting to recover by grabbing again."));

		GameplaySystem = FindComponentByClass<UGameplaySystemComponent>();
	}

	return GameplaySystem;
}

void ACharacterBase::K2_GetGameplayTagSystem_Implementation(FGameplayTagSystem & OutGameplayTagSystem)
{
	if(GameplaySystem)
	{
		OutGameplayTagSystem = GameplaySystem->GetGameplayTagSystemAsRef();
	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("BaseCharacter: No GameplaySystem, cannot get GameplayTagSystem!"));
	}
}

FGameplayTagSystem* ACharacterBase::GetGameplayTagSystem()
{
	return GameplaySystem->GetGameplayTagSystem();
}

bool ACharacterBase::ReserveAttackToken_Implementation(int Amount)
{
	if (AttackTokensCount < Amount)
	{
		return false;
	}

	AttackTokensCount -= Amount;
	return true;
}

void ACharacterBase::ReturnAttackToken_Implementation(int Amount)
{
	AttackTokensCount += Amount;
}

bool ACharacterBase::HasAttackTokensAvailable_Implementation(int Amount) const
{
	return Amount <= AttackTokensCount;
}

ETeam ACharacterBase::GetTeam_Implementation()
{
	return Team;
}


