// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "EnemyBase.h"

#include "DevCommons.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "AAPersistenceSubSystem.h"
#include "AASaveGameTypes.h"
#include "AASaveGame.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	// Called after setup to ensure that PressureComponent is set for IsDead()
	Super::BeginPlay();
	
	PressureComponent = FindComponentByClass<UChargeComponent>();

	if (!PressureComponent)
	{
		UE_LOG(VSLog, Error, TEXT("EnemyBase: Could not find PressureComponent"));
	}

	GameplaySystem->OnAttributeChangedDelegateCollection.GetDelegate(EAttributeType::EAT_Health).AddUniqueDynamic(this, &AEnemyBase::OnHealthChanged);
}

void AEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::RemovedFromWorld)
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
		UAAPersistenceSubsystem* PersistenceSubSystem = GameInstance->GetSubsystem<UAAPersistenceSubsystem>();

		check(PersistenceSubSystem);

		PersistenceSubSystem->RequestSaveObject(this);
	}
}

void AEnemyBase::PostEditImport()
{
	Id.Reset();
}

void AEnemyBase::SaveToObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	FGenericEnemySaveObject SaveObject;
	SaveObject.ValidityKey.MakeValid();
	SaveObject.bIsDead = IsDead_Implementation();
	SaveObject.GameplaySystemSaveObject = GameplaySystem->SaveToObject();
	SaveObject.Transform = GetActorTransform();

	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);
	AASaveGameObject->SaveGenericEnemy(Id, SaveObject);
}

void AEnemyBase::LoadFromObject_Implementation(UGameplaySaveGame* SaveGameObject)
{
	UAASaveGame* AASaveGameObject = CastChecked<UAASaveGame>(SaveGameObject);

	const FGenericEnemySaveObject& SaveObject = AASaveGameObject->LoadGenericEnemy(Id);

	if (!SaveObject.ValidityKey.IsValid())
	{
		return;
	}

	SetActorTransform(SaveObject.Transform, false, nullptr, ETeleportType::ResetPhysics);

	GameplaySystem->LoadFromObject(SaveObject.GameplaySystemSaveObject);

	if (SaveObject.bIsDead)
	{
		OnEnemyKilled(true);
	}

}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UChargeComponent* AEnemyBase::GetPressureComponent()
{
	return PressureComponent;
}

FAAResolvedDamageEvent AEnemyBase::ReceiveDamageEvent_Implementation(FAADamageEvent DamageEvent)
{
	check(PressureComponent);

	const FAAResolvedDamageEvent ResolvedDamageEvent = PressureComponent->ResolveDamageEvent(DamageEvent);

	OnDamageTaken(ResolvedDamageEvent);

	return ResolvedDamageEvent;
}

bool AEnemyBase::IsDead_Implementation()
{
	return bIsEnemyDead;
}

void AEnemyBase::OnHealthChanged(EAttributeType Attribute)
{
	if (bIsEnemyDead)
	{
		return;
	}

	if (GameplaySystem->GetAttributeValue(EAttributeType::EAT_Health) <= 0)
	{
		bIsEnemyDead = true;
		OnDeathDelegate.Broadcast(false);
		return;
	}
}

bool AEnemyBase::SwitchModeNative(EDamageType NewMode)
{
	const FString EffectName = "ModeSwitchEffect";

	EAttributeType PreviousWeakness = UGameplayUtilityBlueprintLibrary::GetCorrespondingWeakness(CurrentMode);
	EAttributeType NewWeakness = UGameplayUtilityBlueprintLibrary::GetCorrespondingWeakness(NewMode);

	const float PreviousDelta = 1 - GameplaySystem->GetAttributeValue(PreviousWeakness);
	const float NewDelta = 1 - GameplaySystem->GetAttributeValue(NewWeakness);

	// Prepare effect that switches the weakness value between the old and new Mode

	// Set both back to a value of 1
	FAttributeEffect ZeroPreviousEffect = {};
	ZeroPreviousEffect.Attribute = PreviousWeakness;
	ZeroPreviousEffect.Value = PreviousDelta;

	FAttributeEffect ZeroNewEffect = {};
	ZeroNewEffect.Attribute = NewWeakness;
	ZeroNewEffect.Value = NewDelta;
	
	// Apply the other modes weakness delta
	FAttributeEffect SwitchPreviousEffect = {};
	SwitchPreviousEffect.Attribute = PreviousWeakness;
	SwitchPreviousEffect.Value = -NewDelta;

	FAttributeEffect SwitchNewEffect = {};
	SwitchNewEffect.Attribute = NewWeakness;
	SwitchNewEffect.Value = -PreviousDelta;

	// Wrap in GameplayEffect
	FActiveGameplayEffect ActiveEffect = {};
	ActiveEffect.DurationType = EDurationType::EDT_Instant;
	ActiveEffect.Name = EffectName;
	ActiveEffect.AttributeEffects.Add(ZeroPreviousEffect);
	ActiveEffect.AttributeEffects.Add(ZeroNewEffect);
	ActiveEffect.AttributeEffects.Add(SwitchPreviousEffect);
	ActiveEffect.AttributeEffects.Add(SwitchNewEffect);

	// Switch mode
	CurrentMode = NewMode;

	// End by applying the Effect
	FGameplayEffectHandle Handle;
	return GameplaySystem->AddGameplayEffectByHandle(ActiveEffect, Handle);
}

