// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "DamageCalculation.h"
#include "GameplayTagDefines.h"
#include "HitboxAsset.generated.h"

/**
 * Assetable data describing a hitbox.
 */
UCLASS()
class VERTICALSLICE_API UHitboxAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- Begin UPrimaryDataAsset Interface
	FPrimaryAssetId GetPrimaryAssetId() const override;
	// --- End UPrimaryDataAsset Interface

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UDamageCalculation> DamageCalculationClass = nullptr;
};

UENUM(BlueprintType)
enum class ETargetMesh : uint8
{
	// Will use Actor location instead, with socket offset applied.
	ETM_None			UMETA(DisplayName = "None"),
	
	ETM_SkeletalMesh	UMETA(DisplayName = "Skeletal Mesh"),

	ETM_Weapon			UMETA(DisplayName = "Weapon"),
};

// Options for a Hitbox socket.
// Property owner are recommended to implement a GetSocketNames function in order to populate SocketName for easier selection.
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FHitboxSocket
{
	GENERATED_BODY();

	FHitboxSocket() = default;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (GetOptions = "GetSocketNames"))
	FName SocketName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FTransform SocketOffset;

	// The mesh we will try to attach the hitbox to.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	ETargetMesh Target = ETargetMesh::ETM_None;

	// The specific mesh we want. Used for multi-WeaponComponent Actors.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 MeshIdentifier = 0;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnHitboxOverlapSignature, const FAAResolvedDamageEvent&, ResolvedEvent);

// Per instance data, that allows for reusability of UHitboxAsset between animations.
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FHitboxParams
{
	GENERATED_BODY();

	FHitboxParams();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UHitboxAsset> HitboxAsset = nullptr;

	// Designates what type of hitbox this represents, which affects what properties it can apply on overlap.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer HitboxType;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DamageEvent")
	FAADamageEventFlags EventFlags;

	// Damaged Actors are tracked and can not be damaged again by the same instance.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DamageEvent")
	bool bCanOnlyHitActorOnce = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DamageEvent")
	FDamageCalculationLocals Locals;

	// Applied on any damaged Actor.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DamageEvent")
	TArray<TSubclassOf<UGameplayEffect>> EffectsOnTarget;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Radius = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float HalfHeight = 0.0f;

	// The socket we attach the hitbox to.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FHitboxSocket Socket;
};
