// Copyright (c) 2025, Heavy Duty Tape Studios. All rights reserved.


#include "GA_Gamma.h"

#include "DevCommons.h"

void UGA_Gamma::ActivateAbility_Implementation(FActiveGameplayAbility& OutActiveGameplayAbility)
{
	Super::ActivateAbility_Implementation(OutActiveGameplayAbility);
}

void UGA_Gamma::EndAbility_Implementation()
{
	Super::EndAbility_Implementation();

	bIsEffectActive = false;

}

void UGA_Gamma::ActivateEffect()
{
	if (bIsEffectActive)
	{
		return;
	}

	AActor* Owner = GetOwningActor();

	// No valid owner
	if (!Owner)
	{
		UE_LOG(VSLog, Error, TEXT("No valid owner for ability %s"), *GetName());
		return;
	}

	UWorld* World = Owner->GetWorld();

	if (!World)
	{
		UE_LOG(VSLog, Error, TEXT("No valid world for ability %s"), *GetName());
		return;
	}

	World->GetWorldSettings()->SetTimeDilation(GlobalTimeSlow);

	// The numerator is the wanted dilation on self 
	float SelfTimeDilation = 1 / GlobalTimeSlow;

	Owner->CustomTimeDilation = SelfTimeDilation;
	
	bIsEffectActive = true;
}

void UGA_Gamma::EndEffect()
{
	if (bIsEffectActive == false)
	{
		return;
	}

	AActor* Owner = GetOwningActor();

	// No valid owner
	if (!Owner)
	{
		UE_LOG(VSLog, Error, TEXT("No valid owner for ability %s"), *GetName());
		return;
	}

	UWorld* World = Owner->GetWorld();

	if (!World)
	{
		UE_LOG(VSLog, Error, TEXT("No valid world for ability %s"), *GetName());
		return;
	}

	// Reset time dilation
	World->GetWorldSettings()->SetTimeDilation(1);

	// Reset custom dilation
	Owner->CustomTimeDilation = 1;

	bIsEffectActive = false;
}
