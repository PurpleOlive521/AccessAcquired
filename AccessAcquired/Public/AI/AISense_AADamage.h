// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISense.h"
#include "DamageEventTypes.h"
#include "AISense_AADamage.generated.h"

class IAIPerceptionListenerInterface;

// Wrapper for a Damage event together with any other data needed for the AISense.
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FAISenseAADamageEvent
{
	GENERATED_BODY()

	// Required for reporting
	typedef class UAISense_AADamage FSenseClass;

	FAISenseAADamageEvent() = default;

	FAISenseAADamageEvent(FAAResolvedDamageEvent InDamageEvent) : DamageEvent(InDamageEvent) {};

	bool IsValid() const;

	// Calculates a relative strength of the event, to allow comparing or prioritising between multiple events.
	float GetEventStrengthForStimulus() const;

	IAIPerceptionListenerInterface* GetTargetActorAsPerceptionListener() const;

	// The resolved damage event that triggered this perception event.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sense")
	FAAResolvedDamageEvent DamageEvent;
};

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UAISense_AADamage : public UAISense
{
	GENERATED_BODY()

public:
	void RegisterEvent(const FAISenseAADamageEvent& Event);
	virtual void RegisterWrappedEvent(UAISenseEvent& PerceptionEvent) override;

	// Reports a damage event to the AI Perception system.
	UFUNCTION(BlueprintCallable, Category = "AI|Perception", meta = (WorldContext = "WorldContextObject"))
	static void ReportDamageEvent(UObject* WorldContextObject, FAAResolvedDamageEvent Event);

protected:
	virtual float Update() override;

	UPROPERTY()
	TArray<FAISenseAADamageEvent> RegisteredEvents;
};
