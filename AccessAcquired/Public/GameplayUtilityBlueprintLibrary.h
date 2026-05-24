// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "CombatTypes.h"
#include "LevelTypes.h"
#include "GameplayTagContainer.h"
#include "AI/AIState.h"

#include "GameplayUtilityBlueprintLibrary.generated.h"

class UForceFeedbackEffect;

UCLASS()
class VERTICALSLICE_API UGameplayUtilityBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// --- Input

	// Does the necessary conversion to map the input to a 2D plane, relative to the current active camera. Maps left-right input to camera-x and up-down to camera-y.
	// Seemingly just maps it to World X and World Y, independently of the camera rotation?
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "Context"), Category = "GameplayUtility|Input")
	static FVector2D CameraMapInput(const UObject* Context, const FVector2D& Input);

	// Maps the input to be screen relative. 
	// E.g, if the player faces to the right relative to the screen, a right input (1,0) will result in a player-forward input (0,1).
	// If the player faces forward relative to the screen a forward input (0,1) will remain the same.
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "Context"), Category = "GameplayUtility|Input")
	static FVector2D MakeInputPlayerRelative(const UObject* Context, const FVector2D& Input);

	// --- Feedback

	// Thin wrapper around ClientPlayForceFeedback that accounts for game settings that disable Gamepad rumble.
	UFUNCTION(BlueprintCallable, Category = "GameplayUtility|Feedback")
	static void PlayForceFeedback(APlayerController* Target, UForceFeedbackEffect* ForceFeedbackEffect, FName Tag, bool bLooping, bool bPlayWhilePaused);

	// --- GameplayTags

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility|GameplayTags")
	static FGameplayTag GetModeAsGameplayTag(const EDamageType& Mode);

	// Returns the mode variation of the given parent tag.
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility|GameplayTags")
	static FGameplayTag GetTagChild(const FGameplayTag& TagParent, EDamageType Mode);

	// --- Attributes

	// Returns the corresponding EAttributeType weakness for a given EDamageType.
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility|Attributes")
	static EAttributeType GetCorrespondingWeakness(const EDamageType& DamageType);

	// Returns the corresponding EAttributeType overheat for a given EDamageType. Returns EAT_NONE for EDamageType::Normal.
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility|Attributes")
	static EAttributeType GetCorrespondingOverheat(const EDamageType& DamageType);

	// Switches the mode of a mode-specific attribute. E.g. InfraredOverheat to UltravioletOverheat. Returns EAttributeType::EAT_NONE if no mode variant exists.
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility|Attributes")
	static EAttributeType ChangeAttributeMode(const EDamageType& DamageType, const EAttributeType& Attribute);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility|Attributes")
	static bool IsModeAttribute(const EAttributeType& Attribute);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility|Attributes")
	static EAttributeType GetOverheatByMode(const EDamageType& Mode);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility|Attributes")
	static EAttributeType GetWeaknessByMode(const EDamageType& Mode);

	// Returns the mode equivalent of the attribute.
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility|Attributes")
	static EDamageType GetCorrespondingMode(const EAttributeType& Attribute);

	// --- Teams

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility")
	static bool AreSameTeam(AActor* First, AActor* Second);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility")
	static bool AreSameTeam_OneEnum(AActor* First, ETeam Second);

	// --- Etc.

	// Helper that gets the mode from the players ModeComponent.
	UFUNCTION(BlueprintPure, BlueprintCallable, meta = (WorldContext = "Context"), Category = "GameplayUtility|Player")
	static EDamageType GetPlayerMode(const UObject* Context);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility")
	static FName ConvertLevelToName(ELevelName Level);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility")
	static FString GetRandomLetter();

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility")
	static FString ConvertAIStateToDisplayName(EAIState State);

	// Returns true if the components is more or less on the floor. 
	// Avoids needing to query IsWalking, which will be incorrect for actions that require Flying mode but are still performed along or on a surface.
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "GameplayUtility")
	static bool IsVeryCloseToFloor(UCharacterMovementComponent* MovementComp);

};
