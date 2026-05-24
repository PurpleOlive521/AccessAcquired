// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponAttachmentAsset.generated.h"

// Where we try to attach the weapon to. Uses the MeshAccessorInterface first.
UENUM(BlueprintType)
enum class EPreferredRoot : uint8
{
	// User override, we use the Weapons current parent to attach to.
	EPR_Manual			UMETA(DisplayName = "Manual"),

	// Attaches to root.
	EPR_Root			UMETA(DisplayName = "Root"),

	// Attaches to the SkeletalMesh.
	EPR_SkeletalMesh	UMETA(DisplayName = "SkeletalMesh"),
};

UCLASS()
class VERTICALSLICE_API UWeaponAttachmentAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- Begin UPrimaryDataAsset Interface
	FPrimaryAssetId GetPrimaryAssetId() const override;
	// --- End UPrimaryDataAsset Interface

	// How we determine what to try to attach to.
	UPROPERTY(EditAnywhere)
	EPreferredRoot PreferredRoot;

	// The socket to attach to.
	UPROPERTY(EditAnywhere)
	FName Socket;

	UPROPERTY(EditAnywhere)
	FTransform SocketOffset;
};
