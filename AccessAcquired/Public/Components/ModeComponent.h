// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageEventTypes.h"
#include "ModeComponent.generated.h"

class UGameplaySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModeSwitchedSignature, EDamageType, NewMode);

/*
 * Component for a Mode-aware Actor. The Actor needs to have a GameplaySystem to own Modes before switching to them.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VERTICALSLICE_API UModeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UModeComponent();

	virtual void BeginPlay() override;

	// Tries to get the ModeComponent from this Actor.
	// Will always be a component search, so cache returned value if possible.
	UFUNCTION(BlueprintCallable)
	static UModeComponent* GetModeComponentFromActor(const AActor* Actor);

	// Returns the active mode of the Actor. If no ModeComponent is found on the Actor, returns EDamageType::EDT_Normal.
	UFUNCTION(BlueprintCallable)
	static EDamageType GetModeFromActor(const AActor* Actor);
	
	// Sets the mode. Trigger VFX and visual changes here, but not actions.
	UFUNCTION(BlueprintCallable, Category = "ModeComponent")
	void SetMode(EDamageType NewMode);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "ModeComponent")
	EDamageType GetMode() const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "ModeComponent")
	bool HasMode(EDamageType Mode);

protected:

	UGameplaySystemComponent* GetGameplaySystem();

	UPROPERTY(EditAnywhere)
	EDamageType CurrentMode = EDamageType::EDT_Normal;

	TWeakObjectPtr<UGameplaySystemComponent> GameplaySystem = nullptr;

public:
	// --- Delegates

	UPROPERTY(BlueprintAssignable, Category = "ModeComponent")
	FOnModeSwitchedSignature OnModeSwitchedDelegate;
};
