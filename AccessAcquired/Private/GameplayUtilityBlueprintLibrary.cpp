// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayUtilityBlueprintLibrary.h"

#include "Attribute.h"
#include "GameplayTagDefines.h"
#include "DevCommons.h"
#include "ReceiveDamageEventInterface.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "ExtendedGameUserSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

FVector2D UGameplayUtilityBlueprintLibrary::CameraMapInput(const UObject* Context, const FVector2D& Input)
{
	if (Context; UWorld* World = Context->GetWorld())
	{
		if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(World, 0))
		{
			FRotator CameraRotation = CameraManager->GetCameraRotation();

			const FVector CameraRight = UKismetMathLibrary::GetRightVector(FRotator(0, CameraRotation.Yaw, CameraRotation.Roll));
			const FVector CameraForward = UKismetMathLibrary::GetForwardVector(FRotator(0, CameraRotation.Yaw, 0));

			FVector DesiredDirection = CameraRight * Input.X + CameraForward * Input.Y;

			return FVector2D(DesiredDirection).GetSafeNormal(); // Intentionally drop Z
		}
	}

	return Input;
}

FVector2D UGameplayUtilityBlueprintLibrary::MakeInputPlayerRelative(const UObject* Context, const FVector2D& Input)
{
	if (Context; UWorld * World = Context->GetWorld())
	{
		if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0); ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World, 0))
		{
			const FRotator ControlRotation = PlayerController->GetControlRotation(); 
			const FVector CameraForward = UKismetMathLibrary::GetForwardVector(FRotator(0.0f, ControlRotation.Yaw, 0.0f));
			const FVector PlayerForward = PlayerCharacter->GetActorForwardVector();
			const FQuat QuatDifference = UKismetMathLibrary::Quat_FindBetweenVectors(CameraForward, PlayerForward);

			const FVector2D FinalInput = UKismetMathLibrary::GetRotated2D(Input, QuatDifference.Euler().Z);
			return FinalInput;
		}
	}

	return Input;
}

void UGameplayUtilityBlueprintLibrary::PlayForceFeedback(APlayerController* Target, UForceFeedbackEffect* ForceFeedbackEffect, FName Tag, bool bLooping, bool bPlayWhilePaused)
{
	check(Target);

	UExtendedGameUserSettings* Settings = UExtendedGameUserSettings::Get();
	if (Settings)
	{
		if (Settings->IsGamepadRumbleEnabled())
		{
			Target->K2_ClientPlayForceFeedback(ForceFeedbackEffect, Tag, bLooping, true, bPlayWhilePaused);
		}
	}
}

FGameplayTag UGameplayUtilityBlueprintLibrary::GetModeAsGameplayTag(const EDamageType& Mode)
{
	switch (Mode)
	{
	case EDamageType::EDT_Normal:
		return GAMEPLAYTAG_Modes_Normal;

	case EDamageType::EDT_Infrared:
		return GAMEPLAYTAG_Modes_Infrared;

	case EDamageType::EDT_Ultraviolet:
		return GAMEPLAYTAG_Modes_Ultraviolet;

	case EDamageType::EDT_Gamma:
		return GAMEPLAYTAG_Modes_Gamma;

	default:
		// There should always be a corresponding type
		checkNoEntry();
		return FGameplayTag();
	}
}

FGameplayTag UGameplayUtilityBlueprintLibrary::GetTagChild(const FGameplayTag& TagParent, EDamageType Mode)
{
	if (TagParent == GAMEPLAYTAG_AbilitySlot_Ability)
	{
		switch (Mode)
		{
		case EDamageType::EDT_Normal:
			return GAMEPLAYTAG_AbilitySlot_Ability_Normal;
		case EDamageType::EDT_Infrared:
			return GAMEPLAYTAG_AbilitySlot_Ability_Infrared;
		case EDamageType::EDT_Ultraviolet:
			return GAMEPLAYTAG_AbilitySlot_Ability_Ultraviolet;
		case EDamageType::EDT_Gamma:
			return GAMEPLAYTAG_AbilitySlot_Ability_Gamma;
		default:
			checkNoEntry(); // There should always be a corresponding type
			return FGameplayTag();
		}
	}

	if (TagParent == GAMEPLAYTAG_AbilitySlot_ModeSwitch)
	{
		switch (Mode)
		{
		case EDamageType::EDT_Normal:
			return GAMEPLAYTAG_AbilitySlot_ModeSwitch_Normal;
		case EDamageType::EDT_Infrared:
			return GAMEPLAYTAG_AbilitySlot_ModeSwitch_Infrared;
		case EDamageType::EDT_Ultraviolet:
			return GAMEPLAYTAG_AbilitySlot_ModeSwitch_Ultraviolet;
		case EDamageType::EDT_Gamma:
			return GAMEPLAYTAG_AbilitySlot_ModeSwitch_Gamma;
		default:
			checkNoEntry(); // There should always be a corresponding type
			return FGameplayTag();
		}
	}

	checkNoEntry(); // No mode variation exists for the given parent tag!
	return FGameplayTag();
}

EAttributeType UGameplayUtilityBlueprintLibrary::GetCorrespondingWeakness(const EDamageType& DamageType)
{
	switch (DamageType)
	{
	case EDamageType::EDT_Normal:
		return EAttributeType::EAT_NormalWeakness;

	case EDamageType::EDT_Infrared:
		return EAttributeType::EAT_InfraredWeakness;

	case EDamageType::EDT_Ultraviolet:
		return EAttributeType::EAT_UltravioletWeakness;

	case EDamageType::EDT_Gamma:
		return EAttributeType::EAT_GammaWeakness;

	default:
		// There should always be a corresponding type
		checkNoEntry();
		return EAttributeType();
	}
}

EAttributeType UGameplayUtilityBlueprintLibrary::GetCorrespondingOverheat(const EDamageType& DamageType)
{
	switch (DamageType)
	{
	case EDamageType::EDT_Normal:
		return EAttributeType::EAT_NONE;

	case EDamageType::EDT_Infrared:
		return EAttributeType::EAT_InfraredOverheat;

	case EDamageType::EDT_Ultraviolet:
		return EAttributeType::EAT_UltravioletOverheat;

	case EDamageType::EDT_Gamma:
		return EAttributeType::EAT_GammaOverheat;

	default:
		// There should always be a corresponding type
		checkNoEntry();
		return EAttributeType();
	}
}

EAttributeType UGameplayUtilityBlueprintLibrary::ChangeAttributeMode(const EDamageType& DamageType, const EAttributeType& Attribute)
{
	switch (Attribute)
	{
	case EAttributeType::EAT_InfraredOverheat:
	case EAttributeType::EAT_UltravioletOverheat:
	case EAttributeType::EAT_GammaOverheat:
		return GetOverheatByMode(DamageType);

	case EAttributeType::EAT_NormalWeakness:
	case EAttributeType::EAT_InfraredWeakness:
	case EAttributeType::EAT_UltravioletWeakness:
	case EAttributeType::EAT_GammaWeakness:
		return GetWeaknessByMode(DamageType);

	default:
		return EAttributeType::EAT_NONE;
	}
}

bool UGameplayUtilityBlueprintLibrary::IsModeAttribute(const EAttributeType& Attribute)
{
	switch (Attribute)
	{
	case EAttributeType::EAT_InfraredOverheat:
	case EAttributeType::EAT_UltravioletOverheat:
	case EAttributeType::EAT_GammaOverheat:
	case EAttributeType::EAT_NormalWeakness:
	case EAttributeType::EAT_InfraredWeakness:
	case EAttributeType::EAT_UltravioletWeakness:
	case EAttributeType::EAT_GammaWeakness:
		return true;

	default:
		return false;
	}

}

EAttributeType UGameplayUtilityBlueprintLibrary::GetOverheatByMode(const EDamageType& Mode)
{
	switch (Mode)
	{
	case EDamageType::EDT_Infrared:
		return EAttributeType::EAT_InfraredOverheat;
	case EDamageType::EDT_Ultraviolet:
		return EAttributeType::EAT_UltravioletOverheat;
	case EDamageType::EDT_Gamma:
		return EAttributeType::EAT_GammaOverheat;

	default:
		return EAttributeType::EAT_NONE;
	}
}

EAttributeType UGameplayUtilityBlueprintLibrary::GetWeaknessByMode(const EDamageType& Mode)
{
	switch (Mode)
	{
	case EDamageType::EDT_Normal:
		return EAttributeType::EAT_NormalWeakness;
	case EDamageType::EDT_Infrared:
		return EAttributeType::EAT_InfraredWeakness;
	case EDamageType::EDT_Ultraviolet:
		return EAttributeType::EAT_UltravioletWeakness;
	case EDamageType::EDT_Gamma:
		return EAttributeType::EAT_GammaWeakness;

	default:
		return EAttributeType::EAT_NONE;
	}
}

EDamageType UGameplayUtilityBlueprintLibrary::GetCorrespondingMode(const EAttributeType& Attribute)
{
	switch (Attribute)
	{
	case EAttributeType::EAT_InfraredWeakness:
	case EAttributeType::EAT_InfraredOverheat:
		return EDamageType::EDT_Infrared;

	case EAttributeType::EAT_UltravioletWeakness:
	case EAttributeType::EAT_UltravioletOverheat:
		return EDamageType::EDT_Ultraviolet;

	case EAttributeType::EAT_GammaWeakness:
	case EAttributeType::EAT_GammaOverheat:
		return EDamageType::EDT_Gamma;

	default:
		checkNoEntry(); // No mode-equivalent exists for the given type!
		return EDamageType::EDT_Normal;
	}
}

bool UGameplayUtilityBlueprintLibrary::AreSameTeam(AActor* First, AActor* Second)
{
	if(!First->Implements<UReceiveDamageEventInterface>() || !Second->Implements<UReceiveDamageEventInterface>())
	{
		// While not correct per definition, returning true will end most checks prematurely.
		return true;
	}

	const ETeam TeamFirst = IReceiveDamageEventInterface::Execute_GetTeam(First);
	const ETeam TeamSecond = IReceiveDamageEventInterface::Execute_GetTeam(Second);

	return TeamFirst == TeamSecond;
}

bool UGameplayUtilityBlueprintLibrary::AreSameTeam_OneEnum(AActor* First, ETeam Second)
{
	if (!First->Implements<UReceiveDamageEventInterface>())
	{
		// While not true, returning false will end most checks prematurely.
		return true;
	}

	const ETeam TeamFirst = IReceiveDamageEventInterface::Execute_GetTeam(First);

	return TeamFirst == Second;
}

EDamageType UGameplayUtilityBlueprintLibrary::GetPlayerMode(const UObject* Context)
{

	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(Context, 0);
	if (!Character)
	{
		return EDamageType::EDT_Normal;
	}

	UModeComponent* ModeComponent = Character->GetComponentByClass<UModeComponent>();
	if (!ModeComponent)
	{
		return EDamageType::EDT_Normal;
	}

	return ModeComponent->GetMode();
}

FName UGameplayUtilityBlueprintLibrary::ConvertLevelToName(ELevelName Level)
{
	FString LevelName;

	switch (Level)
	{
	case ELevelName::ELN_Backstreets_StartArea:
		LevelName = TEXT("Backstreet_StartArea");
		break;

	case ELevelName::ELN_Backstreets_Plaza:
		LevelName = TEXT("Backstreet_Plaza");
		break;

	case ELevelName::ELN_Backstreets_FirstFight:
		LevelName = TEXT("Backstreet_FirstFight");
		break;

	case ELevelName::ELN_Backstreets_VioletTower:
		LevelName = TEXT("Backstreet_VioletTower");
		break;

	case ELevelName::ELN_Backstreets_PowerTest:
		LevelName = TEXT("Backstreet_PowerTest");
		break;

	case ELevelName::ELN_Backstreets_BossArena:
		LevelName = TEXT("Backstreet_BossArena");
		break;

	case ELevelName::ELN_Mindspace:
		LevelName = TEXT("MindspaceLevel");
		break;

	default:
		// There should always be a corresponding type
		checkNoEntry();
		return FName();
	}

	return FName(LevelName);
}

FString UGameplayUtilityBlueprintLibrary::GetRandomLetter()
{
	FString Letters = TEXT("abcdefghijklmnopqrstuvwxyz");
	int32 RandomIndex = FMath::RandRange(0, Letters.Len() - 1);
	return FString(1, &Letters[RandomIndex]);
}

FString UGameplayUtilityBlueprintLibrary::ConvertAIStateToDisplayName(EAIState State)
{
	switch (State)
	{
	case EAIState::AS_Passive :
		return TEXT("Passive");
	case EAIState::AS_Attacking :
		return TEXT("Attacking");
	case EAIState::AS_Frozen:
		return TEXT("Frozen");
	case EAIState::AS_Dead:
		return TEXT("Dead");
	case EAIState::AS_MAX:
		return TEXT("MAX");
	default:
		checkNoEntry();	// There should always be a corresponding type
		return FString();
	}
}

bool UGameplayUtilityBlueprintLibrary::IsVeryCloseToFloor(UCharacterMovementComponent* MovementComp)
{
	if (MovementComp)
	{
		if (AActor* Owner = MovementComp->GetOwner())
		{
			FFindFloorResult Floor;
			MovementComp->FindFloor(Owner->GetActorLocation(), Floor, true /* bCanUseCachedLocation */);

			// We are close to or on the floor
			if (Floor.bWalkableFloor)
			{
				return true;
			}
		}

		if (MovementComp->IsWalking())
		{
			return true;
		}
		
	}

	return false;
}
