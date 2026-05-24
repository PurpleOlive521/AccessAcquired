// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "MaterialTypesBlueprintLibrary.h"

void UMaterialTypesBlueprintLibrary::GenerateAndApplyMaterials(FGeneratedMaterialCollection& MaterialCollection, UMeshComponent* MeshComponent)
{
	MaterialCollection.GenerateMaterials(MeshComponent);
	MaterialCollection.ApplyDynamicMaterials(MeshComponent);
}

void UMaterialTypesBlueprintLibrary::ReapplySourceMaterials(FGeneratedMaterialCollection& MaterialCollection, UMeshComponent* MeshComponent)
{
	MaterialCollection.ApplySourceMaterials(MeshComponent);
}

bool UMaterialTypesBlueprintLibrary::GenerateAndApplyMaterials_Map(FGeneratedMaterialCollectionMap& MaterialCollectionMap, UMeshComponent* MeshComponent)
{
	const bool bWasGenerated = MaterialCollectionMap.GenerateMaterials(MeshComponent);

	if (!bWasGenerated)
	{
		return false;
	}

	return MaterialCollectionMap.ApplyDynamicMaterials(MeshComponent);
}

bool UMaterialTypesBlueprintLibrary::ReapplySourceMaterials_Map(FGeneratedMaterialCollectionMap& MaterialCollectionMap, UMeshComponent* MeshComponent)
{
	return MaterialCollectionMap.ApplySourceMaterials(MeshComponent);
}

void UMaterialTypesBlueprintLibrary::GetDynamicMaterials(UPARAM(ref)FGeneratedMaterialCollectionMap& MaterialCollectionMap, TArray<UMaterialInstanceDynamic*>& OutMaterials)
{
	MaterialCollectionMap.GetDynamicMaterials(OutMaterials);
}
