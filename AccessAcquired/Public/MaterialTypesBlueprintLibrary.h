// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AAMaterialTypes.h"
#include "MaterialTypesBlueprintLibrary.generated.h"

UCLASS()
class VERTICALSLICE_API UMaterialTypesBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Generate Dynamic Material instances for all the materials in the MeshComponent, and replace the original materials with them.
	UFUNCTION(BlueprintCallable, Category = "MaterialTypesBlueprintLibrary")
	static void GenerateAndApplyMaterials(UPARAM(ref) FGeneratedMaterialCollection& MaterialCollection, UMeshComponent* MeshComponent);

	// Apply the original materials that we generated Dynamic Materials from on the MeshComponent.
	UFUNCTION(BlueprintCallable, Category = "MaterialTypesBlueprintLibrary")
	static void ReapplySourceMaterials(UPARAM(ref) FGeneratedMaterialCollection& MaterialCollection, UMeshComponent* MeshComponent);

	// Generate Dynamic Material instances for all the materials in the MeshComponent, and replace the original materials with them.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Generate And Apply Materials for Map"), Category = "MaterialTypesBlueprintLibrary")
	static bool GenerateAndApplyMaterials_Map(UPARAM(ref) FGeneratedMaterialCollectionMap& MaterialCollectionMap, UMeshComponent* MeshComponent);

	// Apply the original materials that we generated Dynamic Materials from on the MeshComponent.
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Reapply Source Materials for Map"), Category = "MaterialTypesBlueprintLibrary")
	static bool ReapplySourceMaterials_Map(UPARAM(ref) FGeneratedMaterialCollectionMap& MaterialCollectionMap, UMeshComponent* MeshComponent);

	UFUNCTION(BlueprintCallable, Category = "MaterialTypesBlueprintLibrary")
	static void GetDynamicMaterials(UPARAM(ref) FGeneratedMaterialCollectionMap& MaterialCollectionMap, TArray<UMaterialInstanceDynamic*>& OutMaterials);

};
