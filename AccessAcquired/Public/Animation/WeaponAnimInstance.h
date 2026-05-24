// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/GSAnimInstance.h"
#include "WeaponAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class VERTICALSLICE_API UWeaponAnimInstance : public UGSAnimInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, Category = "WeaponAnimInstance")
	void OnWeaponEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "WeaponAnimInstance")
	void OnWeaponUnequipped();
};
