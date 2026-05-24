// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "DamageEventModifier.h"
#include "HitboxSourceComponent.h"
#include "GameplaySystemComponent.h"

UDamageEventModifier::UDamageEventModifier()
{
    auto IsFunctionImplementedInBlueprint = [](const UFunction* Func) -> bool
        {
            return Func && ensure(Func->GetOuter()) && Func->GetOuter()->IsA(UBlueprintGeneratedClass::StaticClass());
        };

    {
        static FName FuncName = FName(TEXT("K2_CanApplyModifier"));
        UFunction* BlueprintFunction = GetClass()->FindFunctionByName(FuncName);
        bHasBlueprintCanApplyModifier = IsFunctionImplementedInBlueprint(BlueprintFunction);
    }
}

void UDamageEventModifier::AddModifier(UHitboxSourceComponent& OwningComponent)
{
    HitboxSourceComponent = &OwningComponent;

    GameplaySystem = MakeWeakObjectPtr(HitboxSourceComponent->GetGameplaySystem());

    OnModifierAdded();

    K2_OnModifierAdded();
}

void UDamageEventModifier::RemoveModifier()
{
    OnModifierRemoved();
    
    K2_OnModifierRemoved();

    HitboxSourceComponent = nullptr;
    GameplaySystem.Reset();
}

bool UDamageEventModifier::TryApplyDamageEventModifier(FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams)
{
    const bool bCanApply = CheckCanApplyModifier(DamageEvent, HitboxParams);
    if (bCanApply)
    {
        ApplyDamageEventModifier(DamageEvent, HitboxParams);
        return true;
    }

    return false;
}

bool UDamageEventModifier::CheckCanApplyModifier(const FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams) const
{
    if (not bIsEnabled)
    {
        return false;
    }

    if (not CanApplyModifier(DamageEvent, HitboxParams))
    {
        return false;
    }

    if (bHasBlueprintCanApplyModifier)
    {
        if (not K2_CanApplyModifier(DamageEvent, HitboxParams))
        {
            return false;
        }
    }

    return true;
}

void UDamageEventModifier::ApplyDamageEventModifier(FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams)
{
    ApplyModifier(DamageEvent, HitboxParams);

    K2_ApplyModifier(DamageEvent, HitboxParams);
}

UGameplaySystemComponent* UDamageEventModifier::GetGameplaySystem() const
{
    return GameplaySystem.Get();
}

AActor* UDamageEventModifier::GetInstigator() const
{
    return HitboxSourceComponent ? HitboxSourceComponent->GetInstigator() : nullptr;
}

void UDamageEventModifier::SetIsEnabled(bool bInState)
{
    bIsEnabled = bInState;
}

void UDamageEventModifier::OnModifierAdded()
{

}

void UDamageEventModifier::OnModifierRemoved()
{
}

void UDamageEventModifier::ApplyModifier(FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams)
{
    DamageEvent.Damage          *= DamageScalar;
    DamageEvent.AppliedCharge   *= ChargeScalar;
    DamageEvent.Knockback       *= KnockbackScalar;

    DamageEvent.EffectsOnTarget.Append(EffectsOnTarget);
    DamageEvent.StackModifiersOnTarget.Append(StackModifiersOnTarget);

    if (UGameplaySystemComponent* GameplaySystemPtr = GetGameplaySystem())
    {
        FGameplayEffectHandle Handle;
        for (auto& GameplayEffect : EffectsOnSelf)
        {
            GameplaySystemPtr->AddGameplayEffectFromType(GameplayEffect, Handle, GetInstigator());
        }
    }
}

bool UDamageEventModifier::CanApplyModifier(const FAADamageEvent& DamageEvent, const FHitboxParams& HitboxParams) const
{
    if (not bApplyToAnyHitboxType)
    {
        if (not HitboxParams.HitboxType.HasAny(AllowedHitboxTypes))
        {
            return false;
        }
    }

    return true;
}
