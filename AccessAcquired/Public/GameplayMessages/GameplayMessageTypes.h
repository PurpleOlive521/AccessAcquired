// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "DamageEventTypes.h"

#include "GameplayMessageTypes.generated.h"

constexpr int DEBUG_MAX_CONTEXT_TAGS = 3;

class UResponsiveButtonPromptAsset;

// Represents a generic message of the form 'Instigator Verb Target in Context with Magnitude'.
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FAAVerbMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	FGameplayTag Verb;

	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	TObjectPtr<UObject> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	TObjectPtr<UObject> Target = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	FGameplayTagContainer InstigatorTags;

	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	FGameplayTagContainer TargetTags;

	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	FGameplayTagContainer ContextTags;

	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	float Magnitude = 1.0;

	// Returns a debug string representation of this message
	FString ToString() const;
};

// Represents a message that conveys information about a damage event.
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FAADamageEventMessage
{
	GENERATED_BODY()

	FAADamageEventMessage() = default;

	explicit FAADamageEventMessage(const FAAResolvedDamageEvent& InEvent) : ResolvedDamageEvent(InEvent) {};

	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	FAAResolvedDamageEvent ResolvedDamageEvent;
};

// Represents a message with the rewards from destroying an object or enemy.
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FAARewardMessage
{
	GENERATED_BODY()

	FAARewardMessage() = default;

	// Acredited with the kill or destruction of Target.
	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	AActor* Instigator = nullptr;

	// The Actor that was destroyed or killed.
	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	AActor* Target = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	float GainedExperience = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	int GainedIntel = 0;
};

// Either activates and displays a list of responsive button prompts, or removes displayed ones.
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FAATutorialButtonPromptMessage
{
	GENERATED_BODY()

	FAATutorialButtonPromptMessage() = default;

	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	UResponsiveButtonPromptAsset* ContentAsset = nullptr;

	// Will hide the widget instead.
	UPROPERTY(BlueprintReadWrite, Category = GameplayMessage)
	bool bShouldRemoveDisplay = false;
};
