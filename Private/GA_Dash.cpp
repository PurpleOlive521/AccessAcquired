// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "GA_Dash.h"

#include "GameplaySystemComponent.h"
#include "GameplayTagDefines.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

UGA_Dash::UGA_Dash()
{
	ActivationBlockedTags.AddTag(GAMEPLAYTAG_Status_PerformingAction);
	BlockAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_PrimaryAction);
	CancelAbilitiesWithTag.AddTag(GAMEPLAYTAG_GameplayAbility_Types_PrimaryAction);
}

bool UGA_Dash::CheckAbilityRequirements_Implementation() const
{
	const bool bParentSucceeded = Super::CheckAbilityRequirements_Implementation();

	if (!bParentSucceeded)
	{
		return false;
	}

	return true;
}

bool UGA_Dash::ApplyAbilityRequirements_Implementation()
{
	const bool bParentSucceeded = Super::ApplyAbilityRequirements_Implementation();

	if (!bParentSucceeded)
	{
		return false;
	}

	return true;
}

void UGA_Dash::ActivateAbility_Implementation(FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility_Implementation(OutActiveGameplayAbility);

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_Untargetable);

	// TODO: Stop current montage here if needed later

	// The ability is now considered active, but we want to allow the user some time to input the direction for the dash before we *actually* dash
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance);
	AActor* Owner = GetOwningActor_Checked();

	const float TimeDilationAdjusted = InputDelay / Owner->GetActorTimeDilation();
	GameInstance->GetTimerManager().SetTimer(InputDelayTimer, this, &UGA_Dash::AfterInputDelay, TimeDilationAdjusted);
}

void UGA_Dash::CancelAbility_Implementation()
{
	// TODO: We could also stop the users momentum here if we want? Might not feel very good for the user tho.

	EndAbility_Implementation();

	Super::CancelAbility_Implementation();
}

void UGA_Dash::EndAbility_Implementation()
{
	if (InputDelayTimer.IsValid())
	{
		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
		check(GameInstance);

		GameInstance->GetTimerManager().ClearTimer(InputDelayTimer);
	}

	Super::EndAbility_Implementation();
}

void UGA_Dash::ApplyAbilityEndedModifiers_Implementation()
{
	Super::ApplyAbilityEndedModifiers_Implementation();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->RemoveTag(GAMEPLAYTAG_Status_Untargetable);
}

void UGA_Dash::RemoveAbilityEndedModifiers_Implementation()
{
	Super::RemoveAbilityEndedModifiers_Implementation();

	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();

	GameplaySystem->AddTag(GAMEPLAYTAG_Status_PerformingAction);
	GameplaySystem->AddTag(GAMEPLAYTAG_Status_Untargetable);
}


void UGA_Dash::AfterInputDelay()
{
	UGameplaySystemComponent* GameplaySystem = GetOwningComponent_Checked();
	APlayerCharacter* Player = CastChecked<APlayerCharacter>(GetOwningActor());

	const float MovementSpeed = GameplaySystem->GetAttributeValue(EAttributeType::EAT_MovementSpeed);
	const FVector DashDirection = Player->GetDesiredMovementDirection() * MovementSpeed * DashForceMultiplier;

	Player->LaunchCharacter(DashDirection, true, true);

	// The dash is currently controlled by a blendspace for the actual animation, so we need to explicitly claim any incoming AnimNotifys as ours.
	GameplaySystem->GetAnimMontageInfo()->AssignOverrideAbility(this);

	K2_OnDashStarted();
}
