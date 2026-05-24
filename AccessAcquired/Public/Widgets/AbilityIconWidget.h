// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagDefines.h"
#include "AttributeTypes.h"
#include "GameplayAbilitySlot.h"
#include "CombatTypes.h"
#include "AbilityIconWidget.generated.h"

class ACharacterBase;
class UGameplaySystemComponent;
class UGA_PlayerBase;
class UModeComponent;

UENUM(BlueprintType)
enum class EAbilityState : uint8
{
	// Not yet activatable.
	EAS_NotReady		UMETA(DisplayName = "Not Ready"),

	// Activatable.
	EAS_Ready			UMETA(DisplayName = "Ready"),

	// Currently active.
	EAS_Active			UMETA(DisplayName = "Active"),
};

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UAbilityIconWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	// --- Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	// --- End UUserWidget Interface

	void OnAbilityActivated(TSubclassOf<UGameplayAbility> ActivatedAbility, const FGameplayTag& SlotTag);

	void OnAbilityFinished(TSubclassOf<UGameplayAbility> ActivatedAbility, const FGameplayTag& SlotTag);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On State Changed"), Category = "AbilityIconWidgetS")
	void K2_OnStateChanged();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Activation Progress Updated"))
	void K2_OnActivationProgressUpdated(float Percent);

	UFUNCTION()
	void OnModeSwitched(EDamageType Mode);

	void OnOverheatChanged(EAttributeType ChangedAttribute);

protected:

	void SetState(EAbilityState InState);

	UPROPERTY(BlueprintReadOnly)
	EDamageType CurrentMode = EDamageType::ENUM_MAX;

	UPROPERTY(BlueprintReadOnly)
	EAbilityState AbilityState = EAbilityState::EAS_NotReady;

	FGameplayAbilitySlot* TargetSlot = nullptr;

	float LastOverheat = 0.0f;

	// We cache the cast ability when modified
	TObjectPtr<UGA_PlayerBase> PlayerDerivedAbility = nullptr;
	 
private:
	FDelegateHandle AbilityActivatedHandle;
	FDelegateHandle AbilityFinishedHandle;

	FDelegateHandle OverheatInfraredHandle;
	FDelegateHandle OverheatUltravioletHandle;
	FDelegateHandle OverheatGammaHandle;
	
	TWeakObjectPtr<ACharacterBase> BoundCharacter;
	TWeakObjectPtr<UGameplaySystemComponent> BoundGameplaySystem;
	TWeakObjectPtr<UModeComponent> BoundModeComponent;

};
