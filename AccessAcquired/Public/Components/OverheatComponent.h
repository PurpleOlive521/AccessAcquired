// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeTypes.h"
#include "CombatTypes.h"
#include "GameplayTagContainer.h"
#include "OverheatComponent.generated.h"

class APlayerCharacter;
class UGameplaySystemComponent;

UENUM(BlueprintType)
enum class EOverheatSeverity : uint8
{
	EOS_None			UMETA(DisplayName = "None"),	// Not Overheated
	EOS_Mild			UMETA(DisplayName = "Mild"),
	EOS_Moderate		UMETA(DisplayName = "Moderate"),
	EOS_Severe			UMETA(DisplayName = "Severe"),
	EOS_Critical		UMETA(DisplayName = "Critical"),
};

struct FModeState
{
	FModeState() = default;

	bool bIsOverheating = false;

	bool bIsOverclocking = false;

	EOverheatSeverity Severity = EOverheatSeverity::EOS_None;
};

// Breakpoints for the upper limit to Severities.
constexpr float OVERHEAT_SEVERITY_MILD =		0.25f;
constexpr float OVERHEAT_SEVERITY_MODERATE =	0.50f;
constexpr float OVERHEAT_SEVERITY_SEVERE =		0.75f;
constexpr float OVERHEAT_SEVERITY_CRITICAL =	0.90f;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModeOverheatedSignature, bool, bEntered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeSeveritySignature, EOverheatSeverity, Severity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOverclockedSignature, bool, bIsOverclocked);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VERTICALSLICE_API UOverheatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UOverheatComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	void ListenForOverheat(EAttributeType ChangedAttribute);

	void ListenForGameplayTags(FGameplayTag ModifiedTag, bool bWasAdded);

	void UpdateOverheat(EAttributeType OverheatType);

	void UpdateSeverity(EAttributeType OverheatType);

	void UpdateOverclock(EAttributeType OverheatType);

	void SetSeverity(EDamageType Mode, EOverheatSeverity NewSeverity);

	EOverheatSeverity GetHighestSeverity();

	TMap<EDamageType, FModeState> ModeStates;

	// Usually the highest Severity present across the modes.
	EOverheatSeverity ActiveSeverity = EOverheatSeverity::EOS_None;

	TWeakObjectPtr<UGameplaySystemComponent> GameplaySystem = nullptr;

	// TODO: Redesign such that any Character-derived object, or object with a Mode-type component can use this.
	TWeakObjectPtr<APlayerCharacter> Player = nullptr;

public:
	// --- Delegates

	// Called when the active mode's Overheat passes the limit or when recovered from Overheat.
	UPROPERTY(BlueprintAssignable)
	FOnModeOverheatedSignature OnModeOverheatedDelegate;

	// Called when the Overheat Severity changes.
	UPROPERTY(BlueprintAssignable)
	FOnChangeSeveritySignature OnChangeSeverityDelegate;

	// Called when the entering or exiting Overclocked.
	UPROPERTY(BlueprintAssignable)
	FOnOverclockedSignature OnOverclockedDelegate;
};
