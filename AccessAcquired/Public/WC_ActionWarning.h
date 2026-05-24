// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageEventTypes.h"
#include "WC_ActionWarning.generated.h"

class UActionWarningWidget;
class UModeComponent;
class UAnimNotifyState_SpawnHitbox;

struct FTriggerInfo
{
	FTriggerInfo() = default;

	explicit FTriggerInfo(const UAnimNotifyState_SpawnHitbox* InNotify, float InTriggerTime, FAADamageEventFlags InAdditionalFlags);

	// The start time of the AnimNotify in the AnimMontage.
	float TriggerTime = 0.0f;

	FAADamageEventFlags AdditionalFlags;

	// The AnimNotify that will trigger.
	TWeakObjectPtr<const UAnimNotifyState_SpawnHitbox> Notify = nullptr;
};

struct FTrackedMontage
{
	FTrackedMontage() = default;

	explicit FTrackedMontage(UAnimMontage* Montage);

	TObjectPtr<UAnimMontage> AnimMontage;

	TArray<FTriggerInfo> Triggers;

	friend uint32 GetTypeHash(const FTrackedMontage& InTrackedMontage);

	bool operator==(const FTrackedMontage& Other) const
	{
		return AnimMontage == Other.AnimMontage;
	}

	bool operator!=(const FTrackedMontage& Other) const
	{
		return AnimMontage != Other.AnimMontage;
	}
};

/**
 * Automatically plays ActionWarnings when Hitboxes are created from the owning Actor.
 * The rate (amount of time before) at which they are played can be customized per component.
 */
UCLASS()
class VERTICALSLICE_API UWC_ActionWarning : public UWidgetComponent
{
	GENERATED_BODY()
	
public:

	UWC_ActionWarning(const FObjectInitializer& ObjectInitializer);

	// --- Begin UActorComponent Interface 
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// --- End UActorComponent Interface 

	UFUNCTION(BlueprintCallable)
	void ActivateActionWarning(FAADamageEventFlags Flags);

	UFUNCTION(BlueprintCallable)
	void StopActionWarning();

	UActionWarningWidget* GetWidgetInstance();
	
	UFUNCTION()
	void OnMontageStarted(UAnimMontage* Montage);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// Activates and removes any tracked triggers in the TrackedMontage.
	void ResolveActivatedTriggers(FTrackedMontage& TrackedMontage, float AbsolutePlayRate, float Position);

	// Boiler-plate setup before further calling ResolveActivatedTriggers on the TrackedMontage.
	void ProcessTrackedMontage(UAnimInstance* AnimInstance, FTrackedMontage& TrackedMontage);

	bool ShouldTrigger(const FTriggerInfo& Trigger, float RealTimeUntilTrigger) const;

protected:

	// The amount of time in seconds before an attack starts that the ActionWarning should trigger.
	UPROPERTY(EditAnywhere, Category = "ActionWarning")
	float TriggerTime = 0.5f;

	// Time added to the ActionWarning Unparryable attacks. Added onto TriggerTime.
	UPROPERTY(EditAnywhere, Category = "ActionWarning")
	float UnparryableAdditionalTriggerTime = 0.5f;

	TSet<FTrackedMontage> TrackedMontages;

	TObjectPtr<UActionWarningWidget> WidgetInstance = nullptr;

	TWeakObjectPtr<UModeComponent> ModeComponent = nullptr;

	TWeakObjectPtr<UAnimInstance> AnimInstance = nullptr;
};
