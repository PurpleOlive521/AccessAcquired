// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyAsset.generated.h"

class AEnemyBase;
class USkeletalMesh;

/**
 * Assetable enemy.
 * Avoid hard references, use Soft pointers and load when required.
 */
UCLASS()
class VERTICALSLICE_API UEnemyDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- Begin UPrimaryDataAsset Interface
	FPrimaryAssetId GetPrimaryAssetId() const override;
	// --- End UPrimaryDataAsset Interface

	UPROPERTY(EditAnywhere)
	FText DisplayName = INVTEXT("Enemy");

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> EnemyClass = nullptr;

	// TODO:
	// Use a Editor-only DataAsset that we load in separately to avoid these being loaded in build?

	// Mesh used for visualisation in Levels.
	UPROPERTY(EditAnywhere, Category = "Editor Only")
	TObjectPtr<USkeletalMesh> StandInMesh = nullptr;

	// Place in the same order as the material slots.
	UPROPERTY(EditAnywhere, Category = "Editor Only")
	TArray<TObjectPtr<UMaterialInterface>> MeshMaterials;

	// The pose we use for the Stand-in mesh.
	UPROPERTY(EditAnywhere, Category = "Editor Only")
	TObjectPtr<UAnimSequence> Pose;
};

USTRUCT(BlueprintType)
struct VERTICALSLICE_API FEnemySelector
{
	GENERATED_BODY()

	FEnemySelector() = default;

	UPROPERTY(EditAnywhere)
	UEnemyDataAsset* SelectedAsset = nullptr;
};