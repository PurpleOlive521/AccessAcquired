// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "PlayerCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DevCommons.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "GameplayTagDefines.h"
#include "GameplayTagSystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTagMessageDefines.h"
#include "GameplayMessageTypes.h"
#include "SaveGameFunctionLibrary.h"
#include "GameCurrencySubsystem.h"
#include "UnlockableSkillSubsystem.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	FGameplayTagSystem* TagSystem = GameplaySystem->GetGameplayTagSystem();
	TagSystem->OnGameplayTagChangedDelegate.AddUObject(this, &APlayerCharacter::ListenForGameplayTags);

	FAAVerbMessage SpawnedMsg;
	SpawnedMsg.Target = this;
	SpawnedMsg.TargetTags = TagSystem->GetGameplayTagContainer();
	SpawnedMsg.Verb = GAMEPLAYTAG_Verbs_Spawned;

	UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(this);
	MessageSystem->BroadcastMessage(GAMEPLAYTAG_Channels_GameplayEvents_Player, SpawnedMsg);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITORONLY_DATA

void APlayerCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (not SpawnId.IsValid())
	{
		SpawnId = FGuid::NewGuid();
	}
}

#endif //WITH_EDITORONLY_DATA

int32 APlayerCharacter::GetDefaultComboCounter()
{
	const int32 Count = GameplaySystem->GetTagCount(GAMEPLAYTAG_Player_Combo_DefaultCount);

	if (Count >= AttackAbilities.Num())
	{
		const int32 NewCount = 0;
		GameplaySystem->SetTagCount(GAMEPLAYTAG_Player_Combo_DefaultCount, NewCount);
		return NewCount;
	}

	return Count;
}

int32 APlayerCharacter::GetHeavyComboCounter()
{
	const int32 Count = GameplaySystem->GetTagCount(GAMEPLAYTAG_Player_Combo_HeavyCount);

	if (Count >= HeavyAttackAbilities.Num())
	{
		const int32 NewCount = 0;
		GameplaySystem->SetTagCount(GAMEPLAYTAG_Player_Combo_HeavyCount, NewCount);
		return NewCount;
	}

	return Count;
}

void APlayerCharacter::ListenForGameplayTags(FGameplayTag ModifiedTag, bool bWasAdded)
{
	// Modify combat tag to match aggro count.
	if (ModifiedTag == GAMEPLAYTAG_Player_AggroedEnemy)
	{
		FGameplayTagSystem* TagSystem = GameplaySystem->GetGameplayTagSystem();
		
		if (!bWasAdded)
		{
			TagSystem->RemoveTag(GAMEPLAYTAG_Status_CombatReady_InCombat);
		}
		else if (bWasAdded)
		{
			TagSystem->AddTag(GAMEPLAYTAG_Status_CombatReady_InCombat);
		}

		return;
	}

	if (ModifiedTag == GAMEPLAYTAG_Status_CombatReady_InCombat)
	{
		if (bWasAdded)
		{
			TriggerCombatStarted();
		}
		else
		{
			TriggerCombatEnded();
		}
	}
}

void APlayerCharacter::TriggerCombatStarted()
{
	FGameplayTagSystem* TagSystem = GameplaySystem->GetGameplayTagSystem();

	FAAVerbMessage CombatStartedMsg;
	CombatStartedMsg.Instigator = this;
	CombatStartedMsg.InstigatorTags = TagSystem->GetGameplayTagContainer();
	CombatStartedMsg.Verb = GAMEPLAYTAG_Verbs_EnteredCombat;

	if (DetectionInstigator)
	{
		CombatStartedMsg.Instigator = DetectionInstigator;
		if (UGameplaySystemComponent* InstigatorGS = UGameplaySystemComponent::GetGameplaySystemFromActor(DetectionInstigator))
		{
			CombatStartedMsg.TargetTags = InstigatorGS->GetGameplayTagSystem()->GetGameplayTagContainer();
		}
	}

	UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(this);
	MessageSystem->BroadcastMessage(GAMEPLAYTAG_Channels_Combat_Player, CombatStartedMsg);

	K2_OnEnterCombat();
}

void APlayerCharacter::TriggerCombatEnded()
{
	FGameplayTagSystem* TagSystem = GameplaySystem->GetGameplayTagSystem();

	FAAVerbMessage CombatExitedMsg;
	CombatExitedMsg.Target = this;
	CombatExitedMsg.TargetTags = GameplaySystem->GetGameplayTagSystem()->GetGameplayTagContainer();
	CombatExitedMsg.Verb = GAMEPLAYTAG_Verbs_ExitedCombat;

	UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(this);
	MessageSystem->BroadcastMessage(GAMEPLAYTAG_Channels_Combat_Player, CombatExitedMsg);

	K2_OnExitCombat();
}

bool APlayerCharacter::ReserveActionTokens_Implementation(int Amount)
{
	// We adjust the amount of available attack tokens based on the amount of enemies currently in combat with the player
	// This should make it more dynamic, with smaller groups staying a relatively small threat while bigger groups become more overwhelming

	if (Amount <= 0)
	{
		return true;
	}

	int NewAttackTokenMax = 0;
	const int Hostiles = GameplaySystem->GetGameplayTagSystem()->GetTagCount(GAMEPLAYTAG_Player_AggroedEnemy);

	if (Hostiles >= HIGH_ENEMY_COUNT)
	{
		NewAttackTokenMax = HIGH_ENEMY_ATTACK_TOKENS;
	}
	else if (Hostiles >= MEDIUM_ENEMY_COUNT)
	{
		NewAttackTokenMax = MEDIUM_ENEMY_ATTACK_TOKENS;
	}
	else
	{
		NewAttackTokenMax = LOW_ENEMY_ATTACK_TOKENS;
	}

	if (NewAttackTokenMax != MaxActionTokens)
	{
		int Delta = NewAttackTokenMax - MaxActionTokens;
		ActionTokenCounter += Delta;

		MaxActionTokens = NewAttackTokenMax;
		ActionTokenCounter = FMath::Clamp(ActionTokenCounter, 0, MaxActionTokens);
	}

	if (ActionTokenCounter < Amount)
	{
		return false;
	}

	ActionTokenCounter -= Amount;
	return true;
}

void APlayerCharacter::ReturnActionTokens_Implementation(int Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	ActionTokenCounter += Amount;
	ActionTokenCounter = FMath::Clamp(ActionTokenCounter, 0, MaxActionTokens);
}

bool APlayerCharacter::OnSerialize(FSaveGameArchive& Archive, bool bIsLoading)
{
	bool bSuccess = USaveGameFunctionLibrary::SerializeActorTransform(Archive, this);

	bSuccess &= USaveGameFunctionLibrary::SerializeGenericController(Archive, GetController());
	bSuccess &= GameplaySystem->OnSerialize(Archive, bIsLoading);
	bSuccess &= HitboxSource->OnSerialize(Archive, bIsLoading);
	bSuccess &= UGameCurrencySubsystem::Get(this)->OnSerialize(Archive, bIsLoading);
	bSuccess &= UUnlockableSkillSubsystem::Get(this)->OnSerialize(Archive, bIsLoading);

	return bSuccess;
}

const FGuid APlayerCharacter::GetSpawnID_Implementation() const
{
	return SpawnId;
}

bool APlayerCharacter::SetSpawnID_Implementation(const FGuid& NewID)
{
	SpawnId = NewID;
	return true;
}

bool APlayerCharacter::IsGlobalData_Implementation()
{
	return true;
}

FAAResolvedDamageEvent APlayerCharacter::ResolveDamageEvent(const FAADamageEvent& DamageEvent)
{
	// Setting the default, base behaviour
	FAAResolvedDamageEvent ResolvedDamageEvent(DamageEvent, this);
	ResolvedDamageEvent.EventStates.bWasEventIgnored = false;
	ResolvedDamageEvent.EventStates.bShouldPlayHitAnimation = true;
	ResolvedDamageEvent.AppliedKnockback = DamageEvent.Knockback;

	if (IReceiveDamageEventInterface::Execute_IsDead(this) == true)
	{
		ResolvedDamageEvent.ResolveAsAlreadyDead();
		return ResolvedDamageEvent;
	}

	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_KnockbackImmunity))
	{
		ResolvedDamageEvent.AppliedKnockback = 0.0f;
	}

	// --- Check for perfect dodge
	if (not DamageEvent.EventFlags.bIsUnparryable)
	{
		if (DamageEvent.DamageType == ModeComponent->GetMode() && GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_CanPerfectDodge) == true)
		{
			ResolvedDamageEvent.ResolveAsPerfectParried();
			return ResolvedDamageEvent;
		}
	}

	// --- Check against tags

	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_Untargetable))
	{
		ResolvedDamageEvent.ResolveAsUntargetable();
		return ResolvedDamageEvent;
	}

	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_Invincible))
	{
		ResolvedDamageEvent.ResolveAsInvincible(DamageEvent.EventFlags.bAllowHitAnim);
		return ResolvedDamageEvent;
	}

	if(!DamageEvent.EventFlags.bIgnoreIFrames)
	{
		if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_IFrames))
		{
			ResolvedDamageEvent.ResolveAsInvincible(DamageEvent.EventFlags.bAllowHitAnim);
			return ResolvedDamageEvent;
		}
	}

	if (GameplaySystem->GetGameplayTagSystem()->HasTag(GAMEPLAYTAG_Status_HyperArmor))
	{
		ResolvedDamageEvent.ResolveAsUninterruptible();
	}

	// --- Calculate new attribute values

	const float DamageReduction = GameplaySystem->GetAttributeValue(EAttributeType::EAT_DamageReduction, EAttributeValue::EAV_CurrentValue);
	const float FinalDamageReduction = FMath::Abs(1.0f - (DamageReduction / 100.0f));

	const float WeaknessMultiplier = GameplaySystem->GetAttributeValue(UGameplayUtilityBlueprintLibrary::GetCorrespondingWeakness(DamageEvent.DamageType), EAttributeValue::EAV_CurrentValue);

	const float FinalDamage = DamageEvent.Damage * WeaknessMultiplier * FinalDamageReduction;
	const float FinalCharge = DamageEvent.AppliedCharge * FinalDamageReduction;

	GameplaySystem->ModifyAttributeValue(EAttributeType::EAT_Health, EAttributeValue::EAV_BaseValue, -FinalDamage);
	GameplaySystem->ModifyAttributeValue(EAttributeType::EAT_Charge, EAttributeValue::EAV_BaseValue, FinalCharge);
	ResolvedDamageEvent.DamageTaken = FinalDamage;
	ResolvedDamageEvent.ChargeAdded = FinalCharge;

	// Apply GameplayEffects
	FGameplayEffectHandle Handle;
	for (TSubclassOf<UGameplayEffect> Effect : DamageEvent.EffectsOnTarget)
	{
		GameplaySystem->AddGameplayEffectFromType(Effect, Handle, DamageEvent.DamageSource);
	}

	// Apply GameplayEffectStackModifiers
	for (const auto& GameplayEffectModifier : DamageEvent.StackModifiersOnTarget)
	{
		GameplaySystem->ApplyGameplayEffectStackModifier(GameplayEffectModifier.GameplayEffect, GameplayEffectModifier.Modifier, DamageEvent.DamageSource);
	}

	return ResolvedDamageEvent;
}

FAAResolvedDamageEvent APlayerCharacter::ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent)
{
	FAAResolvedDamageEvent ResolvedDamageEvent = ResolveDamageEvent(DamageEvent);

	if (ResolvedDamageEvent.AppliedKnockback > 0.0f)
	{
		FVector DirectionToSource = ResolvedDamageEvent.Source->GetActorLocation() - GetActorLocation();
		DirectionToSource.Z = 0.0f;
		const FVector Strength = -DirectionToSource.GetSafeNormal(0.00001) * ResolvedDamageEvent.AppliedKnockback;
		LaunchCharacter(Strength, true /* bXYOverride*/, false /* bZOverride */);
	}

	K2_OnDamageEventResolved(ResolvedDamageEvent);

	FAADamageEventMessage EventMessage = FAADamageEventMessage(ResolvedDamageEvent);
	UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem->BroadcastMessage(GAMEPLAYTAG_Channels_Combat_DamageEvents, EventMessage);
	
	return ResolvedDamageEvent;
}
