// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "EnemyBase.h"

#include "DevCommons.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AISense_AADamage.h"
#include "Perception/AISense_Hearing.h"
#include "EnemySpawnpoint.h"
#include "GameplayTagDefines.h"
#include "EnemyAIControllerBase.h"
#include "GameplayMessageTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTagMessageDefines.h"
#include "SaveGameFunctionLibrary.h"

AEnemyBase::AEnemyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	ChargeComponent = ObjectInitializer.CreateDefaultSubobject<UChargeComponent>(this, TEXT("ChargeComponent"));

	TargetLockableComponent = ObjectInitializer.CreateDefaultSubobject<UTargetLockableComponent>(this, TEXT("TargetLockableComponent"));
	TargetLockableComponent->SetIsLockableInCombat(true);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	FGameplayTagSystem* TagSystem = GameplaySystem->GetGameplayTagSystem();
	TagSystem->OnGameplayTagChangedDelegate.AddUObject(this, &AEnemyBase::ListenForGameplayTags);
}

bool AEnemyBase::ShouldAlwaysTrackDestroyed_Implementation()
{
	return true;
}

bool AEnemyBase::OnSerialize(FSaveGameArchive& Archive, bool bIsLoading)
{
	bool bSuccess = USaveGameFunctionLibrary::SerializeActorTransform(Archive, this);

	bSuccess &= GameplaySystem->OnSerialize(Archive, bIsLoading);

	return bSuccess;
}

void AEnemyBase::OnDying()
{
	Super::OnDying();
}

void AEnemyBase::OnDead(bool bIsOnLoad)
{
	Super::OnDead(bIsOnLoad);

	if (bIsOnLoad)
	{
		Destroy();
		return;
	}

	if (UGameplayMessageSubsystem::HasInstance(this))
	{
		FAARewardMessage RewardMessage;
		RewardMessage.Instigator = UGameplayStatics::GetPlayerCharacter(this, 0);
		RewardMessage.Target = this;

		if (GameplaySystem)
		{
			const int Level = GameplaySystem->GetEntityLevel();

			const float ExperienceRandom = FMath::FRandRange(EXPERIENCE_RANDOMIZE_MIN, EXPERIENCE_RANDOMIZE_MAX);
			RewardMessage.GainedExperience = ExperienceRandom * Level * EXPERIENCE_GAINED_PER_LEVEL;

			const float IntelRandom = FMath::FRandRange(INTEL_RANDOMIZE_MIN, INTEL_RANDOMIZE_MAX);
			RewardMessage.GainedIntel = IntelRandom * Level * INTEL_GAINED_PER_LEVEL;
		}

		UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(this);
		MessageSystem->BroadcastMessage(GAMEPLAYTAG_Channels_Combat_Rewards, RewardMessage);
	}

	OnEnemyDeadDelegate.Broadcast(this);
	Destroy();
}

AEnemyAIControllerBase* AEnemyBase::GetAIController()
{
	if (!AIController)
	{
		AIController = Cast<AEnemyAIControllerBase>(GetController());
	}

	return AIController;
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UChargeComponent* AEnemyBase::GetChargeComponent()
{
	return ChargeComponent;
}

FAAResolvedDamageEvent AEnemyBase::ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent)
{
	check(ChargeComponent);

	const FAAResolvedDamageEvent ResolvedDamageEvent = ChargeComponent->ResolveDamageEvent(DamageEvent);

	UAISense_AADamage::ReportDamageEvent(this, ResolvedDamageEvent);

	const FVector NoiseLocation = ResolvedDamageEvent.HitLocation;
	AActor* Source = ResolvedDamageEvent.Source;
	UAISense_Hearing::ReportNoiseEvent(this, NoiseLocation, DAMAGE_TAKEN_LOUDNESS, Source, DAMAGE_TAKEN_MAX_RANGE);

	if (ResolvedDamageEvent.AppliedKnockback > 0.0f) 
	{
		FVector DirectionToSource = ResolvedDamageEvent.Source->GetActorLocation() - GetActorLocation();
		DirectionToSource.Z = 0.0f;
		const FVector Strength = -DirectionToSource.GetSafeNormal() * ResolvedDamageEvent.AppliedKnockback;
		LaunchCharacter(Strength, true /* bXYOverride*/, false /* bZOverride */);
	}

	K2_OnDamageTaken(ResolvedDamageEvent);

	FAADamageEventMessage EventMessage = FAADamageEventMessage(ResolvedDamageEvent);
	UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem->BroadcastMessage(GAMEPLAYTAG_Channels_Combat_DamageEvents, EventMessage);

	return ResolvedDamageEvent;
}

void AEnemyBase::SetSpawnpoint(AEnemySpawnpoint* InSpawnpoint)
{
	check(InSpawnpoint);

	Spawnpoint = MakeWeakObjectPtr(InSpawnpoint);
}

AEnemySpawnpoint* AEnemyBase::GetSpawnpoint()
{
	return Spawnpoint.Get();
}

void AEnemyBase::ListenForGameplayTags(FGameplayTag ModifiedTag, bool bWasAdded)
{
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

		return;
	}
}

void AEnemyBase::TriggerCombatStarted()
{
	// We want to alert nearby enemies that we have found a hostile.
	AEnemyAIControllerBase* AI = GetAIController();
	if (!AI)
	{
		UE_LOG(VSLog, Error, TEXT("AEnemyBase: Could not get AIController!"));
		return;
	}

	AActor* CurrentTarget = AI->GetCurrentTarget();
	if (CurrentTarget)
	{
		const FVector NoiseLocation = GetActorLocation();
		AActor* Source = CurrentTarget;
		UAISense_Hearing::ReportNoiseEvent(this, NoiseLocation, DAMAGE_TAKEN_LOUDNESS, Source, DAMAGE_TAKEN_MAX_RANGE);
	}

	FAAVerbMessage Message;
	Message.Verb = GAMEPLAYTAG_Verbs_EnteredCombat;
	Message.InstigatorTags = GameplaySystem->GetGameplayTagSystem()->GetGameplayTagContainer();
	Message.Instigator = this;
	Message.Target = CurrentTarget;

	UGameplayMessageSubsystem* MessageSystem = &UGameplayMessageSubsystem::Get(this);
	MessageSystem->BroadcastMessage(GAMEPLAYTAG_Channels_Combat_Enemy, Message);

	K2_OnEnterCombat();
}

void AEnemyBase::TriggerCombatEnded()
{
	K2_OnExitCombat();
}