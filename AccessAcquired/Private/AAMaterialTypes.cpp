// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AAMaterialTypes.h"

#include "Components/MeshComponent.h"

FGeneratedMaterialCollection::FGeneratedMaterialCollection(UMeshComponent* MeshComponent)
{
	GenerateMaterials(MeshComponent);
}

void FGeneratedMaterialCollection::GenerateMaterials(UMeshComponent* MeshComponent)
{
	check(MeshComponent);

	TArray<UMaterialInterface*> Materials = MeshComponent->GetMaterials();

	// We do not want users to combine multiple material sources in one object since we use index to map to their original material slot.
	SourceMaterials.Empty();
	DynamicMaterials.Empty();

	for(UMaterialInterface* Material : Materials)
	{
		SourceMaterials.Add(Material);
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, MeshComponent);
		DynamicMaterials.Add(DynamicMaterial);
	}

}

bool FGeneratedMaterialCollection::ApplyDynamicMaterials(UMeshComponent* MeshComponent)
{
	check(MeshComponent);

	if(DynamicMaterials.Num() == 0)
	{
		return false;
	}

	for (int i = 0; i < DynamicMaterials.Num(); i++)
	{
		MeshComponent->SetMaterial(i, DynamicMaterials[i]);
	}

	return true;
}

bool FGeneratedMaterialCollection::ApplySourceMaterials(UMeshComponent* MeshComponent)
{
	check(MeshComponent);

	if (SourceMaterials.Num() == 0)
	{
		return false;
	}

	for (int i = 0; i < SourceMaterials.Num(); i++)
	{
		MeshComponent->SetMaterial(i, SourceMaterials[i]);
	}

	return true;
}

bool FGeneratedMaterialCollectionMap::GenerateMaterials(UMeshComponent* MeshComponent)
{
	check(MeshComponent);

	FGeneratedMaterialCollection* ExistingCollection = MaterialCollectionsMap.Find(MeshComponent);

	if(ExistingCollection)
	{
		return false;
	}

	FGeneratedMaterialCollection NewCollection;
	NewCollection.GenerateMaterials(MeshComponent);
	MaterialCollectionsMap.Add(MeshComponent, NewCollection);
	return true;
}

bool FGeneratedMaterialCollectionMap::ApplyDynamicMaterials(UMeshComponent* MeshComponent)
{
	check(MeshComponent);

	FGeneratedMaterialCollection* ExistingCollection = MaterialCollectionsMap.Find(MeshComponent);

	if(!ExistingCollection)
	{
		return false;
	}

	return ExistingCollection->ApplyDynamicMaterials(MeshComponent);
}

bool FGeneratedMaterialCollectionMap::ApplySourceMaterials(UMeshComponent* MeshComponent)
{
	check(MeshComponent);

	FGeneratedMaterialCollection* ExistingCollection = MaterialCollectionsMap.Find(MeshComponent);

	if (!ExistingCollection)
	{
		return false;
	}

	return ExistingCollection->ApplySourceMaterials(MeshComponent);
}

void FGeneratedMaterialCollectionMap::GetDynamicMaterials(TArray<UMaterialInstanceDynamic*>& OutMaterials) const
{
	for (const auto& [MeshComponent, MaterialCollection] : MaterialCollectionsMap)
	{
		OutMaterials.Append(MaterialCollection.DynamicMaterials);
	}
}
