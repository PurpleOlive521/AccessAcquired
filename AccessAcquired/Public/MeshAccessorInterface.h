// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MeshAccessorInterface.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;
class UWeaponComponent;

UINTERFACE(MinimalAPI)
class UMeshAccessorInterface : public UInterface
{
	GENERATED_BODY()
};

class VERTICALSLICE_API IMeshAccessorInterface
{
	GENERATED_BODY()

public:

	// Gets the Weapon by Index, if one exists. Can return nullptr.
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "MeshAccessorInterface")
	UWeaponComponent* GetWeapon(int32 Index = 0);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "MeshAccessorInterface")
	USkeletalMeshComponent* GetSkeletalMesh();
};
