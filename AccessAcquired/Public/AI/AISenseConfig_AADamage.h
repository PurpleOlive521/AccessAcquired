// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISenseConfig.h"
#include "AISense_AADamage.h"
#include "AISenseConfig_AADamage.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "AI AADamage sense config"))
class VERTICALSLICE_API UAISenseConfig_AADamage : public UAISenseConfig
{
	GENERATED_BODY()
	
public:
	UAISenseConfig_AADamage(const FObjectInitializer& ObjectInitializer);

	virtual TSubclassOf<UAISense> GetSenseImplementation() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", NoClear, config)
	TSubclassOf<UAISense_AADamage> Implementation;
};
