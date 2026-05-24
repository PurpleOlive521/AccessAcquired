// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitboxAsset.h"
#include "GameplayTagContainer.h"
#include "DamageEventTypes.h"
#include "DamageEventModifier.h"
#include "SaveableObjectInterface.h"
#include "HitboxSourceComponent.generated.h"

class UGameplaySystemComponent;

constexpr uint32 INVALID_HITBOX_HANDLE_ID = 0U;

// Shares implementation with GameplayEventHandle.h
// Could be moved to use a ID per HitboxSourceComponent, if the amount we consume over time is considerate and cross-component comparisons are unlikely.
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FSpawnedHitboxHandle
{
	GENERATED_BODY()

	FSpawnedHitboxHandle() : Id(), bWasInitialized(false) {};

	// Does not ensure that NewId is unused or unique.
	FSpawnedHitboxHandle(uint32 NewId) : Id(NewId), bWasInitialized(true) {};

	static FSpawnedHitboxHandle CreateNew();

	void GenerateNewHandle();

	[[nodiscard]] bool IsValid() const;

	friend uint32 GetTypeHash(const FSpawnedHitboxHandle& InHandle);

	bool operator==(const FSpawnedHitboxHandle& Other) const
	{
		return Id == Other.Id;
	}

	bool operator!=(const FSpawnedHitboxHandle& Other) const
	{
		return Id != Other.Id;
	}

private:

	uint32 Id = 0U;

	uint32 bWasInitialized : 1;
};

USTRUCT()
struct VERTICALSLICE_API FSpawnedHitbox
{
	GENERATED_BODY()

	FSpawnedHitbox() = default;

	FSpawnedHitbox(const FHitboxParams& InParams, UShapeComponent* Instance, const FOnHitboxOverlapSignature& Delegate);

	// Will reattach the SpawnedHitbox if possible to update state.
	void UpdateParams(const FHitboxParams& InParams);

	void Destroy();

	void GenerateDamageEvent(AActor* Instigator, UGameplaySystemComponent* InstigatorGameplaySystem);

	FHitboxParams Params;

	FOnHitboxOverlapSignature OnHitboxOverlapDelegate;

	FAADamageEvent PendingEvent;

	UPROPERTY()
	TObjectPtr<UShapeComponent> SpawnedHitbox = nullptr;

	TArray<TObjectPtr<AActor>> HitActors;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitResolvedSignature, const FAAResolvedDamageEvent&, Event);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGenerateDamageEventSignature, FAADamageEvent&, DamageEvent, const FHitboxParams&, HitboxParams);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VERTICALSLICE_API UHitboxSourceComponent : public UActorComponent, public ISaveableObjectInterface
{
	GENERATED_BODY()

public:
	UHitboxSourceComponent();

	virtual void BeginPlay() override;

	static UShapeComponent* CreateHitbox_Sphere(AActor* Actor, const FHitboxParams& Params);

	static UShapeComponent* CreateHitbox_Capsule(AActor* Actor, const FHitboxParams& Params);

	static USkeletalMeshComponent* GetTargetMeshFromActor(AActor* Actor, ETargetMesh TargetMesh, int32 Identifier = 0);

	// Attaches the hitbox to the OptionalMesh, or Actor root if it is nullptr. 
	// Unattaches first in case we are calling this to update HitboxParams and not only as a first time attachment.
	static bool AttachHitbox(const FHitboxParams& Params, UShapeComponent* Hitbox, AActor* Actor);

	// --- Begin SaveableObject Interface
	virtual bool OnSerialize(FSaveGameArchive& Archive, bool bIsLoading) override;
	// --- End SaveableObject Interface

	// Overrides the source to InInstigator.
	UFUNCTION(BlueprintCallable, Category = "HitboxSourceComponent")
	void SetInstigator(AActor* InInstigator);

	FSpawnedHitbox* GetHitboxFromHandle(const FSpawnedHitboxHandle& Handle);

	UFUNCTION(BlueprintCallable, Category = "HitboxSourceComponent")
	UShapeComponent* GetInstanceFromHandle(const FSpawnedHitboxHandle& Handle);

	FSpawnedHitboxHandle CreateHitbox(AActor* Actor, const FHitboxParams& Params, FOnHitboxOverlapSignature OverlapDelegate = FOnHitboxOverlapSignature());

	bool DestroyHitbox(const FSpawnedHitboxHandle& Handle);

	UFUNCTION()
	void OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHitboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "HitboxSourceComponent")
	AActor* GetInstigator() const;

	UFUNCTION(BlueprintCallable, Category = "HitboxSourceComponent")
	UGameplaySystemComponent* GetGameplaySystem();

	// Returns the created DamageEventModifier.
	UFUNCTION(BlueprintCallable, Category = "HitboxSourceComponent|DamageEvents")
	UDamageEventModifier* AddDamageEventModifier(TSubclassOf<UDamageEventModifier> ModifierClass);

	UFUNCTION(BlueprintCallable, Category = "HitboxSourceComponent|DamageEvents")
	bool RemoveDamageEventModifierByType(TSubclassOf<UDamageEventModifier> ModifierClass);

	UFUNCTION(BlueprintCallable, Category = "HitboxSourceComponent|DamageEvents")
	bool RemoveDamageEventModifier(UDamageEventModifier* Modifier);

	UFUNCTION(BlueprintCallable, Category = "HitboxSourceComponent|DamageEvents")
	UDamageEventModifier* GetDamageEventModifierByType(TSubclassOf<UDamageEventModifier> ModifierClass);

	// Applies any active modifiers on the Hitbox's pending DamageEvent.
	// Returns the modified copy of the pending DamageEvent.
	FAADamageEvent ApplyDamageEventModifiers(const FSpawnedHitbox* Hitbox);

protected:

	FSpawnedHitboxHandle CreateHitbox_Internal(AActor* Actor, const FHitboxParams& Params, FOnHitboxOverlapSignature OverlapDelegate);

	void ProcessOverlappedHitbox(const FSpawnedHitboxHandle& OverlappedHandle, AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& SweepResult);

	// Returns an invalid handle if none is found.
	FSpawnedHitboxHandle GetHandleFromPrimitive(UPrimitiveComponent* ComponentToFind);

	// Does an in-place sweep to find overlapping colliders. Uses the Hitboxes own collision parameters for the sweep.
	void GetHitboxOverlaps(const FSpawnedHitboxHandle& Handle, TArray<FHitResult>& HitResults);

	TSubclassOf<UDamageEventModifier> GetDefaultModifierClass() const;

	UDamageEventModifier* GetDefaultModifier();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitboxSourceComponent")
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECollisionChannel::ECC_WorldDynamic;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HitboxSourceComponent|DamageEvents")
	TSubclassOf<UDamageEventModifier> DefaultModifierClass = UDamageEventModifier::StaticClass();

	TSet<FSpawnedHitboxHandle> OverlappingHitboxes;

	TMap<FSpawnedHitboxHandle, FSpawnedHitbox> SpawnedHitboxes;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UDamageEventModifier>> AppliedModifiers;

	UPROPERTY(Transient)
	TObjectPtr<UDamageEventModifier> DefaultModifier;

	TWeakObjectPtr<AActor> OverrideInstigator = nullptr;

	TWeakObjectPtr<UGameplaySystemComponent> GameplaySystem = nullptr;

public:

	// Broadcasted every time we resolve a DamageEvent generated from a Hitbox.
	UPROPERTY(BlueprintAssignable, Category = "HitboxSourceComponent")
	FOnHitResolvedSignature OnHitResolvedDelegate;

	// Broadcasted when a DamageEvent is generated - either when a overlap occurs or when a hitbox is created.
	// Allows for modifying the DamageEvent before its applied, usually based on the accompanying HitboxParams.
	UPROPERTY(BlueprintAssignable, Category = "HitboxSourceComponent|DamageEvents")
	FOnGenerateDamageEventSignature OnGenerateDamageEventDelegate;
};