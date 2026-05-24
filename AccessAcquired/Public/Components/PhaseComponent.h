// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Attribute.h"
#include "PhaseComponent.generated.h"

class UGameplaySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangedPhaseSignature, int32, NewPhase);

USTRUCT(BlueprintType)
struct VERTICALSLICE_API FPhaseThreshold
{
	GENERATED_BODY()

	FPhaseThreshold() = default;

	UPROPERTY(EditAnywhere, meta = (UIMin = 2))
	int Phase = 2;

	// At what percent of health do we trigger this phase. 0 to 100 value.
	UPROPERTY(EditAnywhere, meta = (UIMin = 0, UIMax = 100, Units = "Percent"), Category = "Trigger")
	float Health = 0.0f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VERTICALSLICE_API UPhaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPhaseComponent();

	virtual void BeginPlay() override;


#if WITH_EDITORONLY_DATA

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif // WITH_EDITORONLY_DATA

	UFUNCTION(BlueprintCallable)
	void SetPhase(int32 Phase);

protected:

	UFUNCTION()
	void ListenForGameplayTags(FGameplayTag GameplayTag, int NewCount, int Delta);

	void ListenForHealth(EAttributeType ChangedAttribute);

	void SetPhase_Internal(int32 Phase);

	// When a PhaseThresholds requirements are met.
	void PhaseTriggered(FPhaseThreshold& Threshold);

	UPROPERTY(EditAnywhere)
	TArray<FPhaseThreshold> PhaseThresholds;

	TWeakObjectPtr<UGameplaySystemComponent> GameplaySystem;

	int32 CurrentPhase = 1;

public:
	// --- Delegates
	UPROPERTY(BlueprintAssignable)
	FOnChangedPhaseSignature OnChangePhaseDelegate;
};
