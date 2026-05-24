// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AAMaterialTypes.generated.h"

class UMeshComponent;

// A collection of dynamic materials and the source material they are generated from.
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FGeneratedMaterialCollection
{
	GENERATED_BODY()

	FGeneratedMaterialCollection() = default;

	[[nodiscard]] FGeneratedMaterialCollection(UMeshComponent* MeshComponent);

	// Generates dynamic materials for the given MeshComponent based on its current materials.
	void GenerateMaterials(UMeshComponent* MeshComponent);

	// Applies the generated dynamic materials (if any) on the given MeshComponent.
	// Returns false if no generated materials exist.
	bool ApplyDynamicMaterials(UMeshComponent* MeshComponent);

	// Reapplies the source materials on the given MeshComponent.
	// Returns false if no source materials exist.
	bool ApplySourceMaterials(UMeshComponent* MeshComponent);

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> SourceMaterials;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;
};

// Supports multiple collections of generated materials mapped by the source MeshComponent (or derivate).
USTRUCT(BlueprintType)
struct VERTICALSLICE_API FGeneratedMaterialCollectionMap
{
	GENERATED_BODY()

	FGeneratedMaterialCollectionMap() = default;

	// Generates dynamic materials for the given MeshComponent based on its current materials.
	// Returns false if already generated in this map.
	bool GenerateMaterials(UMeshComponent* MeshComponent);

	// Applies the generated dynamic materials (if any) on the given MeshComponent.
	// Returns false if no generated materials are found for the MeshComponent.
	bool ApplyDynamicMaterials(UMeshComponent* MeshComponent);

	// Reapplies the source materials on the given MeshComponent.
	// Returns false if no generated materials are found for the MeshComponent.
	bool ApplySourceMaterials(UMeshComponent* MeshComponent);

	void GetDynamicMaterials(TArray<UMaterialInstanceDynamic*>& OutMaterials) const;

	UPROPERTY()
	TMap<TObjectPtr<UMeshComponent>, FGeneratedMaterialCollection> MaterialCollectionsMap;
};