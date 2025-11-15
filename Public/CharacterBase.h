// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "GameplayTagSystem.h"
#include "ReceiveDamageEventInterface.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameplaySystemComponent.h"

#include "CharacterBase.generated.h"

UCLASS()
class VERTICALSLICE_API ACharacterBase : public ACharacter, public IReceiveDamageEventInterface, public IGameplaySystemOwnerInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();

	// --- Begin AActor Interface
	virtual void BeginPlay() override;

	// --- End AActor Interface

	// --- Begin IGameplaySystemOwnerInterface
	UGameplaySystemComponent* GetGameplaySystemComponent_Implementation() override;

	void K2_GetGameplayTagSystem_Implementation(FGameplayTagSystem& OutGameplayTagSystem) override;

	virtual FGameplayTagSystem* GetGameplayTagSystem() override;
	// --- End IGameplaySystemOwnerInterface

	// --- Begin IReceiveDamageEventInterface
	bool ReserveAttackToken_Implementation(int Amount) override;

	void ReturnAttackToken_Implementation(int Amount) override;

	bool HasAttackTokensAvailable_Implementation(int Amount) const override;

	ETeam GetTeam_Implementation() override;
	// --- End IReceiveDamageEventInterface
protected:

	// Bound to OnAttributeChanged to update on invoked changes
	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	void MapMovementValues(EAttributeType ChangedAttribute);

	UPROPERTY(BlueprintReadOnly, Category = "DamageEvents")
	int AttackTokensCount = 1;

	UPROPERTY(BlueprintReadOnly, Category = "DamageEvents")
	int MaxAttackTokens = 1;

	// Edit this rather than the component default, this overwrites defaults at runtime
	UPROPERTY(EditDefaultsOnly, Category = "CharacterBase")
	float MaxAcceleration = 1000.f;

	// Edit this rather than the component default, this overwrites defaults at runtime
	UPROPERTY(EditDefaultsOnly, Category = "CharacterBase")
	float GroundFriction = 12.f;

	// Edit this rather than the component default, this overwrites defaults at runtime
	UPROPERTY(EditDefaultsOnly, Category = "CharacterBase")
	float MaxWalkingSpeed = 500.f;

	// Dictates how AI perceives and how damage is handled towards this Character
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "DamageEvents", meta = (ExposeOnSpawn = "true"))
	ETeam Team = ETeam::ET_Enemy;

	TObjectPtr<UGameplaySystemComponent> GameplaySystem;

	TObjectPtr<UCharacterMovementComponent> CharacterMovement;

};
