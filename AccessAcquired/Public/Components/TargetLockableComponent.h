// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TargetLockableComponent.generated.h"

class UGameplaySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VERTICALSLICE_API UTargetLockableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTargetLockableComponent();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UFUNCTION(BlueprintCallable)
	void BindToOwner();

	UFUNCTION(BlueprintCallable)
	void UnbindFromOwner();

	// Request any TargetLockingComponent to stop locking on this component.
	UFUNCTION(BlueprintCallable)
	void RequestStopLock();

	UFUNCTION(BlueprintCallable)
	void SetIsLockable(bool InLockable);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsLockable() const;

	UFUNCTION(BlueprintCallable)
	void SetIsLockableInCombat(bool InLockableInCombat);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsLockableInCombat() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetDesiredIconLocation() const;

protected:

	void ListenForDeathTags(FGameplayTag ModifiedTag, bool bWasAdded);

	// Will we be considered as a target if the TargetLockingComponent's owner is in combat.
	UPROPERTY(EditAnywhere, Category = "TargetLockable")
	bool bLockableInCombat = false;

	UPROPERTY(EditAnywhere, Category = "TargetLockable")
	bool bIsLockable = true;

	// Optional Socket to use for placement of targeting icon. Will fallback to Actor center if None is used.
	UPROPERTY(EditAnywhere, Category = "TargetLockable")
	FName SocketToTarget = FName();

	TObjectPtr<USkeletalMeshComponent> SkeletalMesh = nullptr;

	TWeakObjectPtr<UGameplaySystemComponent> GameplaySystem = nullptr;

};
