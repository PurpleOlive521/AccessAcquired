// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayEffects/GEAR_IsInMode.h"
#include "DevelopmentTypes.h"
#include "GameplaySystemComponent.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "PlayerCharacter.h"
#include "ModeComponent.h"
#include "DevCommons.h"

bool UGEAR_IsInMode::CanApply(const FGameplayEffectConditionParams& Params) const
{
    FAIL_ON_FAILED_SUPER(CanApply(Params))

    AActor* TargetActor = Params.GameplaySystem->GetOwner();

    if (!TargetActor)
    {
        UE_LOG(VSLog, Warning, TEXT("Owning Actor could not be accessed from UGEAR_IsInMode!"));
        return false;
    }

    UModeComponent* ModeComponent = TargetActor->GetComponentByClass<UModeComponent>();
    if (!ModeComponent)
    {
        UE_LOG(VSLog, Warning, TEXT("Target does not have a ModeComponent, exiting UGEAR_IsInMode."));
        return false;
    }

    const bool bIsInMode = ModeComponent->GetMode() == RequiredMode;

    if (bInvertValue)
    {
        return !bIsInMode;
    }

    return bIsInMode;
}
