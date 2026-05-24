// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "CharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "DevCommons.h"
#include "GameplayTagDefines.h"

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	GameplaySystem = ObjectInitializer.CreateDefaultSubobject<UGameplaySystemComponent>(this, TEXT("GameplaySystem"));
	ModeComponent = ObjectInitializer.CreateDefaultSubobject<UModeComponent>(this, TEXT("Mode Component"));
	HitboxSource = ObjectInitializer.CreateDefaultSubobject<UHitboxSourceComponent>(this, TEXT("HitboxSourceComponent"));

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		MovementComp->MaxAcceleration = 1000.0f;
		MovementComp->GroundFriction = 12.0f;
		MovementComp->MaxWalkSpeed = 500.0f;
	}
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	MaxAccelerationReference = MovementComp->GetMaxAcceleration();
	GroundFrictionReference = MovementComp->GroundFriction;
	MaxWalkSpeedReference = MovementComp->MaxWalkSpeed;

	ensure(GameplaySystem);
	
	MapMovementValues(EAttributeType::EAT_MovementSpeed);
	GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_MovementSpeed).AddUObject(this, &ACharacterBase::MapMovementValues);

	FGameplayTagSystem& TagSystem = GameplaySystem->GetGameplayTagSystemAsRef();
	TagSystem.OnGameplayTagChangedDelegate.AddUObject(this, &ACharacterBase::ListenForDeathTags);

	GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_Health).AddUObject(this, &ACharacterBase::ListenForAttributes);
	GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_TimeDilation).AddUObject(this, &ACharacterBase::ListenForAttributes);

	AbilitySlots.Init(GameplaySystem);
}

UGameplaySystemComponent* ACharacterBase::GetGameplaySystemComponent_Implementation() const
{
	check(GameplaySystem);

	return GameplaySystem;
}

void ACharacterBase::K2_GetGameplayTagSystem_Implementation(FGameplayTagSystem& OutGameplayTagSystem) const
{
	check(GameplaySystem);

	OutGameplayTagSystem = GameplaySystem->GetGameplayTagSystemAsRef();
}

FGameplayTagSystem* ACharacterBase::GetGameplayTagSystem() const
{
	check(GameplaySystem);

	return GameplaySystem->GetGameplayTagSystem();
}

bool ACharacterBase::ReserveActionTokens_Implementation(int Amount)
{
	if (Amount <= 0)
	{
		return true;
	}

	if (ActionTokenCounter < Amount)
	{
		return false;
	}

	ActionTokenCounter -= Amount;
	return true;
}

void ACharacterBase::ReturnActionTokens_Implementation(int Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	ActionTokenCounter += Amount;
}

bool ACharacterBase::HasActionTokensAvailable_Implementation(int Amount) const
{
	if (Amount <= 0)
	{
		return true;
	}

	return Amount <= ActionTokenCounter;
}

bool ACharacterBase::IsDead_Implementation()
{
	return GameplaySystem->HasTag(GAMEPLAYTAG_Status_Dying) || GameplaySystem->HasTag(GAMEPLAYTAG_Status_Dead);
}

ETeam ACharacterBase::GetTeam_Implementation()
{
	return Team;
}

void ACharacterBase::OnDying()
{
	if (GameplaySystem)
	{
		TArray<FGameplayAbilityHandle> ActiveAbilities;
		GameplaySystem->GetAllActiveAbilities(ActiveAbilities);

		GameplaySystem->CancelAbilities(ActiveAbilities);

		FGameplayAbilitySlot* DeathSlot = AbilitySlots.GetSlot(GAMEPLAYTAG_AbilitySlot_Death);
		if (DeathSlot)
		{
			FGameplayAbilityActivationData ActivationData;
			DeathSlot->ActivateAbility(ActivationData);
		}
	}
}

void ACharacterBase::OnDead(bool bIsOnLoad)
{

}

FGameplayAbilitySlotContainer* ACharacterBase::GetAbilitySlots()
{
	return &AbilitySlots;
}

UModeComponent* ACharacterBase::GetModeComponent()
{
	return ModeComponent;
}

void ACharacterBase::ListenForDeathTags(FGameplayTag ModifiedTag, bool bWasAdded)
{
	// We don't try to reconcille or undo deaths, so we don't care about removal of the tags yet.
	if (!bWasAdded)
	{
		return;
	}

	if (ModifiedTag == GAMEPLAYTAG_Status_Dying)
	{
		OnDying();
		K2_OnDying();
	}
	else if (ModifiedTag == GAMEPLAYTAG_Status_Dead)
	{
		OnDead(false /* bIsOnLoad */);
		K2_OnDead(false /* bIsOnLoad */);
	}
}

void ACharacterBase::ListenForAttributes(EAttributeType ChangedAttribute)
{
	if (IsDead_Implementation())
	{
		return;
	}

	switch (ChangedAttribute)
	{
		case EAttributeType::EAT_Health:
		{
			OnHealthChanged();
			break;
		}

		case EAttributeType::EAT_TimeDilation:
		{
			OnTimeDilationChanged();
			break;
		}
	}
}

void ACharacterBase::OnHealthChanged()
{
	const float Health = GameplaySystem->GetAttributeValue(EAttributeType::EAT_Health, EAttributeValue::EAV_CurrentValue);

	if (Health <= 0.0f)
	{
		GameplaySystem->AddTag(GAMEPLAYTAG_Status_Dying);
	}
}

void ACharacterBase::OnTimeDilationChanged()
{
	const float TimeDilation = GameplaySystem->GetAttributeValue(EAttributeType::EAT_TimeDilation, EAttributeValue::EAV_CurrentValue);

	CustomTimeDilation /= LastTimeDilation;
	CustomTimeDilation *= TimeDilation;

	LastTimeDilation = TimeDilation;
}

void ACharacterBase::MapMovementValues(EAttributeType ChangedAttribute)
{
	float MovementSpeed = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MovementSpeed, EAttributeValue::EAV_CurrentValue);

	// Scalars to keep acceleration and friction consistent to the new speed, ensuring a consistent game-feel as MovementSpeed increases or decreases
	float AccelerationScalar = MaxAccelerationReference / MaxWalkSpeedReference;
	float FrictionScalar = GroundFrictionReference / MaxWalkSpeedReference;

	UCharacterMovementComponent* MovementComp = GetCharacterMovement();

	MovementComp->MaxWalkSpeed = MovementSpeed; // For AI movement
	MovementComp->MaxAcceleration = MovementSpeed * AccelerationScalar;
	MovementComp->GroundFriction = MovementSpeed * FrictionScalar;
}

