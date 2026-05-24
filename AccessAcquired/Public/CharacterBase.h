// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "GameplayTagSystem.h"
#include "ReceiveDamageEventInterface.h"
#include "GameplaySystemOwnerInterface.h"
#include "GameplaySystemComponent.h"
#include "ModeComponent.h"
#include "GameplayAbilitySlot.h"
#include "HitboxSourceComponent.h"

#include "CharacterBase.generated.h"

UCLASS()
class VERTICALSLICE_API ACharacterBase : public ACharacter, public IReceiveDamageEventInterface, public IGameplaySystemOwnerInterface
{
	GENERATED_BODY()

public:

	ACharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	// --- Begin IGameplaySystemOwnerInterface
	virtual UGameplaySystemComponent* GetGameplaySystemComponent_Implementation() const override;

	virtual void K2_GetGameplayTagSystem_Implementation(FGameplayTagSystem& OutGameplayTagSystem) const override;

	virtual FGameplayTagSystem* GetGameplayTagSystem() const override;
	// --- End IGameplaySystemOwnerInterface

	// --- Begin IReceiveDamageEventInterface
	virtual bool ReserveActionTokens_Implementation(int Amount) override;

	virtual void ReturnActionTokens_Implementation(int Amount) override;

	virtual bool HasActionTokensAvailable_Implementation(int Amount) const override;

	virtual bool IsDead_Implementation() override;

	virtual ETeam GetTeam_Implementation() override;
	// --- End IReceiveDamageEventInterface

	virtual void OnDying();

	virtual void OnDead(bool bIsOnLoad);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "On Dying"))
	void K2_OnDying();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DisplayName = "On Dead"))
	void K2_OnDead(bool bIsOnLoad);

	FGameplayAbilitySlotContainer* GetAbilitySlots();

	UFUNCTION(BlueprintCallable, Category = "CharacterBase")
	UModeComponent* GetModeComponent();

	UFUNCTION()
	void ListenForDeathTags(FGameplayTag ModifiedTag, bool bWasAdded);

	void ListenForAttributes(EAttributeType ChangedAttribute);

	void OnHealthChanged();

	void OnTimeDilationChanged();

protected:

	// Bound to OnAttributeChanged to update on invoked changes
	UFUNCTION()
	void MapMovementValues(EAttributeType ChangedAttribute);

	UPROPERTY(BlueprintReadOnly, Category = "DamageEvents")
	int ActionTokenCounter = 1;

	UPROPERTY(BlueprintReadOnly, Category = "DamageEvents")
	int MaxActionTokens = 1;

	// Dictates how AI perceives and how damage is handled towards this Character
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat")
	ETeam Team = ETeam::ET_Enemy;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat")
	FGameplayAbilitySlotContainer AbilitySlots;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UGameplaySystemComponent> GameplaySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UModeComponent> ModeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UHitboxSourceComponent> HitboxSource;

private:

	float MaxAccelerationReference = 0.0f;

	float GroundFrictionReference = 0.0f;

	float MaxWalkSpeedReference = 0.0f;

	float LastTimeDilation = 1.0f;
};
