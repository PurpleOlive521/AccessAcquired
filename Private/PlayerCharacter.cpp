// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "PlayerCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "AttackData.h" 
#include "DevCommons.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "GameplayTagDefines.h"
#include "GameplayTagSystem.h"

constexpr float MINIMUM_ALLOWED_INPUT = 0.1f;
constexpr float TIME_IN_MODE = 5.0f; // Seconds

constexpr int HIGH_ENEMY_COUNT = 6;
constexpr int MEDIUM_ENEMY_COUNT = 4;

constexpr int HIGH_ENEMY_ATTACK_TOKENS = 3;
constexpr int MEDIUM_ENEMY_ATTACK_TOKENS = 2;
constexpr int LOW_ENEMY_ATTACK_TOKENS = 1;

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

FVector APlayerCharacter::GetCameraAdjustedMovementInput() const
{
	FRotator CameraRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraRotation();

	const FVector ForwardAdjusted = UKismetMathLibrary::GetForwardVector(CameraRotation) * LatestMovementInput.Y;
	const FVector RightAdjusted = UKismetMathLibrary::GetRightVector(CameraRotation) * LatestMovementInput.X;

	return ForwardAdjusted + RightAdjusted;;
}

// Remember to drop Z to not allow any vertical movement due to camera angle
FVector APlayerCharacter::GetDesiredMovementDirection() const
{
	// Use the forward direction if the player is not actively giving a direction
	if (LatestMovementInput.Length() < MINIMUM_ALLOWED_INPUT)
	{
		FVector Forward = GetActorForwardVector();
		Forward.Z = 0;
		return Forward;
	}

	const FRotator ControlRotation = GetControlRotation();
	const FVector CameraRight = UKismetMathLibrary::GetRightVector(FRotator(0, ControlRotation.Yaw, ControlRotation.Roll));
	const FVector CameraForward = UKismetMathLibrary::GetForwardVector(FRotator(0 , ControlRotation.Yaw, 0));

	FVector DesiredDirection = CameraRight * LatestMovementInput.X + CameraForward * LatestMovementInput.Y;
	DesiredDirection.Normalize();

	DesiredDirection.Z = 0;

	return DesiredDirection;
}

void APlayerCharacter::AssignDamageEvent(const FAADamageEvent& DamageEvent)
{
	CurrentDamageEvent = DamageEvent;
}

FAADamageEvent APlayerCharacter::GetDamageEvent() const
{
	return CurrentDamageEvent;
}

void APlayerCharacter::IncrementAttackCounter()
{
	AttackCounter++;

	AttackCounter = AttackCounter % AttackAbilities.Num();
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool APlayerCharacter::IsDead_Implementation()
{
	return bIsPlayerDead;
}

bool APlayerCharacter::ReserveAttackToken_Implementation(int Amount)
{
	// We adjust the amount of available attack tokens based on the amount of enemies currently in combat with the player
	// This should make it more dynamic, with smaller groups staying a relatively small threat while bigger groups become more overwhelming

	int NewAttackTokenMax = 0;
	if (HostileActors.Num() >= HIGH_ENEMY_COUNT)
	{
		NewAttackTokenMax = HIGH_ENEMY_ATTACK_TOKENS;
	}
	else if (HostileActors.Num() >= MEDIUM_ENEMY_COUNT)
	{
		NewAttackTokenMax = MEDIUM_ENEMY_ATTACK_TOKENS;
	}
	else
	{
		NewAttackTokenMax = LOW_ENEMY_ATTACK_TOKENS;
	}

	if (NewAttackTokenMax != MaxAttackTokens)
	{
		int Delta = NewAttackTokenMax - MaxAttackTokens;
		AttackTokensCount += Delta;

		MaxAttackTokens = NewAttackTokenMax;
		AttackTokensCount = FMath::Clamp(AttackTokensCount, 0, MaxAttackTokens);
	}

	if (AttackTokensCount < Amount)
	{
		return false;
	}

	AttackTokensCount -= Amount;
	return true;
}

void APlayerCharacter::ReturnAttackToken_Implementation(int Amount)
{
	AttackTokensCount += Amount;
	AttackTokensCount = FMath::Clamp(AttackTokensCount, 0, MaxAttackTokens);
}

FAAResolvedDamageEvent APlayerCharacter::ResolveDamageEvent(const FAADamageEvent& DamageEvent)
{
	// Setting the default, base behaviour
	FAAResolvedDamageEvent ResolvedDamageEvent(DamageEvent);
	ResolvedDamageEvent.EventStates.bWasEventIgnored = false;
	ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = true;

	if (IReceiveDamageEventInterface::Execute_IsDead(this) == true)
	{
		ResolvedDamageEvent.ResolveAsAlreadyDead();
		return ResolvedDamageEvent;
	}

	// --- Check for perfect dodge
	if (DamageEvent.DamageType == GetMode() && GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_CanPerfectDodge) == true)
	{
		ModifyEnergyByAction(EEnergyModifyingAction::EMA_PerfectDodge);

		ResolvedDamageEvent.ResolveAsPerfectDodged();
		return ResolvedDamageEvent;
	}

	// --- Check against tags

	// Take no damage, dont get interrupted
	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_Untargetable))
	{
		ResolvedDamageEvent.ResolveAsUntargetable();
		return ResolvedDamageEvent;
	}

	// Take no damage, still get interrupted
	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_Invincible))
	{
		ResolvedDamageEvent.ResolveAsInvincible();
		return ResolvedDamageEvent;
	}

	// Only check if we wont ignore it regardless of if IFrames are present
	if(!DamageEvent.bIgnoreIFrames)
	{
		if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_IFrames))
		{
			ResolvedDamageEvent.ResolveAsInvincible();
			return ResolvedDamageEvent;
		}
	}

	// Take damage, dont get interrupted
	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_HyperArmor))
	{
		ResolvedDamageEvent.ResolveAsUninterruptible();
	}

	// --- Calculate new attribute values

	const float DamageReduction = FMath::Abs(1 - (GameplaySystem->GetAttributeValue(EAttributeType::EAT_DamageReduction) / 100));

	const float WeaknessMultiplier = GameplaySystem->GetAttributeValue(UGameplayUtilityBlueprintLibrary::GetCorrespondingWeakness(DamageEvent.DamageType));

	float Damage = DamageEvent.Damage * WeaknessMultiplier * DamageReduction;
	float Charge = DamageEvent.AppliedCharge * DamageReduction;

	GameplaySystem->ModifyAttributeBaseValue(EAttributeType::EAT_Health, -Damage);
	GameplaySystem->ModifyAttributeBaseValue(EAttributeType::EAT_Charge, +Charge);
	ResolvedDamageEvent.DamageTaken = Damage;
	ResolvedDamageEvent.ChargeAdded = Charge;

	// Clamp new values
	GameplaySystem->ClampAttributeBaseValue(EAttributeType::EAT_Charge, 0.0f, GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxCharge));
	GameplaySystem->ClampAttributeBaseValue(EAttributeType::EAT_Health, 0.0f, GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxHealth));

	// Apply GameplayEffects
	FGameplayEffectHandle Handle;
	for (TSubclassOf<UGameplayEffect> Effect : DamageEvent.EffectsOnTarget)
	{
		GameplaySystem->AddGameplayEffectFromType(Effect, Handle);
	}

	ModifyEnergyByAction(EEnergyModifyingAction::EMA_HitByEnemy);

	// Check if player died as a result of the attack
	if (GameplaySystem->GetAttributeValue(EAttributeType::EAT_Health) <= 0)
	{
		bIsPlayerDead = true;
		OnDeath();
	}

	return ResolvedDamageEvent;
}

void APlayerCharacter::ModifyEnergy(float Delta, bool bInterruptIdleness)
{
	GameplaySystem->ModifyAttributeBaseValue(EAttributeType::EAT_Energy, Delta);

	const float MaxEnergy = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MaxEnergy);
	GameplaySystem->ClampAttributeBaseValue(EAttributeType::EAT_Energy, 0, MaxEnergy);

	if (bInterruptIdleness)
	{
		InterruptIdleness();
	}
}

void APlayerCharacter::SetMode(EDamageType InMode)
{
	TSubclassOf<UGameplayAbility>* AbilityValue = ModeSwitchAbilities.Find(InMode);

	check(AbilityValue);

	FActiveGameplayAbility ActiveAbility;
	const bool bActivatedAbility = GameplaySystem->UseAbility(*AbilityValue, ActiveAbility);

	if (!bActivatedAbility)
	{
		return;
	}

	SetModeInternal(InMode);
}

void APlayerCharacter::SetModeInternal(EDamageType InMode)
{
	if(CurrentMode == InMode)
	{
		return;
	}

	CurrentMode = InMode;

	GetWorld()->GetTimerManager().ClearTimer(ModeSwitchDurationHandle);
	if (CurrentMode != EDamageType::EDT_Normal)
	{
		GetWorld()->GetTimerManager().SetTimer(ModeSwitchDurationHandle, this, &APlayerCharacter::ResetMode, TIME_IN_MODE);
	}

	// Give Blueprint a chance to apply VFX, animations or state changes before the delegate is called
	K2_SetModeInternal();

	OnModeSwitchedDelegate.Broadcast(CurrentMode);
}

void APlayerCharacter::ResetMode()
{
	SetModeInternal(EDamageType::EDT_Normal);

	K2_ResetMode();
}

EDamageType APlayerCharacter::GetMode() const
{
	return CurrentMode;
}

bool APlayerCharacter::HasMode(EDamageType Mode)
{
	const FGameplayTag ModeAvailabilityTag = UGameplayUtilityBlueprintLibrary::GetAvailableModeAsGameplayTag(Mode);
	if (GetGameplayTagSystem()->HasTag(ModeAvailabilityTag))
	{
		return true;
	}

	return false;
}

float APlayerCharacter::GetModeDurationRemaining() const
{
	const float TimeElapsed = GetWorld()->GetTimerManager().GetTimerElapsed(ModeSwitchDurationHandle);
	const float TimeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(ModeSwitchDurationHandle);

	return TimeElapsed / TimeElapsed + TimeRemaining;
}

bool APlayerCharacter::IsPlayerInCombat() const
{
	return bIsInCombat;
}

FAAResolvedDamageEvent APlayerCharacter::ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent)
{
	FAAResolvedDamageEvent ResolvedDamageEvent = ResolveDamageEvent(DamageEvent);

	K2_OnDamageEventResolved(ResolvedDamageEvent);
	
	return ResolvedDamageEvent;
}

void APlayerCharacter::ModifyEnergyByAction(EEnergyModifyingAction Action)
{
	const float DeltaAdjustedIdleTick = UGameplayStatics::GetWorldDeltaSeconds(this) * -5.0f;

	switch (Action)
	{
	case EEnergyModifyingAction::EMA_AttackHit:
		ModifyEnergy(6.0f, true);
		break;
	case EEnergyModifyingAction::EMA_PerfectDodge:
		ModifyEnergy(20.0f, true);
		break;
	case EEnergyModifyingAction::EMA_AttackHitWeakness:
		ModifyEnergy(3.0f, true);
		break;
	case EEnergyModifyingAction::EMA_HitByEnemy:
		ModifyEnergy(-10.0f, true);
		break;
	case EEnergyModifyingAction::EMA_IdleTick:
		ModifyEnergy(DeltaAdjustedIdleTick, false);
		break;
	case EEnergyModifyingAction::EMA_None:
		break;

	default:
		// Always need to define a value
		checkNoEntry();
		break;
	}
}

bool APlayerCharacter::RequestEnergyUse(float EnergyToUse)
{
	if (EnergyToUse <= GameplaySystem->GetAttributeValue(EAttributeType::EAT_Energy))
	{
		ModifyEnergy(-EnergyToUse, true);
		return true;
	}
	else
	{
		return false;
	}
}

void APlayerCharacter::InterruptIdleness()
{
	bIsIdle = false;

	// Clear the idleness-timer if it has been set.
	if (IdlenessCountdownHandle.IsValid() == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(IdlenessCountdownHandle);
	}

	// Start timer to set player back to idle unless interrupted.
	GetWorld()->GetTimerManager().SetTimer(IdlenessCountdownHandle, this, &APlayerCharacter::SetToIdle, MaxIdleTime, false);
}

void APlayerCharacter::SetToIdle()
{
	bIsIdle = true;

	// Clear the idleness-timer if it has been set.
	if (IdlenessCountdownHandle.IsValid() == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(IdlenessCountdownHandle);
	}
}
