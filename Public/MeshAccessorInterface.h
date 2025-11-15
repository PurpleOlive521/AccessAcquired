// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MeshAccessorInterface.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMeshAccessorInterface : public UInterface
{
	GENERATED_BODY()
};

class VERTICALSLICE_API IMeshAccessorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "MeshAccessorInterface")
	UStaticMeshComponent* GetWeaponMesh();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "MeshAccessorInterface")
	USkeletalMeshComponent* GetWeaponSkeletalMesh();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "MeshAccessorInterface")
	USkeletalMeshComponent* GetSkeletalMesh();
};
