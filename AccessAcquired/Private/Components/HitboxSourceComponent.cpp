// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Components/HitboxSourceComponent.h"
#include "DevelopmentTypes.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "GameplaySystemOwnerInterface.h"
#include "DevCommons.h"
#include "GameplaySystemComponent.h"
#include "WeaponComponent.h"
#include "MeshAccessorInterface.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTagMessageDefines.h"
#include "GameplayMessageTypes.h"

FSpawnedHitboxHandle FSpawnedHitboxHandle::CreateNew()
{
    FSpawnedHitboxHandle Handle = {};
    Handle.GenerateNewHandle();
    return Handle;
}

void FSpawnedHitboxHandle::GenerateNewHandle()
{
    static uint32 NEXT_ID = 1U;

    Id = NEXT_ID++;

    bWasInitialized = true;
}

bool FSpawnedHitboxHandle::IsValid() const
{
    return bWasInitialized && Id != INVALID_HITBOX_HANDLE_ID;
}

uint32 GetTypeHash(const FSpawnedHitboxHandle& InHandle)
{
    return GetTypeHash(InHandle.Id);
}

FSpawnedHitbox::FSpawnedHitbox(const FHitboxParams& InParams, UShapeComponent* Instance, const FOnHitboxOverlapSignature& Delegate)
{
    Params = InParams;

    SpawnedHitbox = Instance;

    OnHitboxOverlapDelegate = Delegate;
}

void FSpawnedHitbox::UpdateParams(const FHitboxParams& InParams)
{
    if (SpawnedHitbox)
    {
        Params = InParams;
        AActor* Owner = SpawnedHitbox->GetOwner();
        UHitboxSourceComponent::AttachHitbox(Params, SpawnedHitbox, Owner);

        if (USphereComponent* Sphere = Cast<USphereComponent>(SpawnedHitbox))
        {
            Sphere->SetSphereRadius(Params.Radius);
            return;
        }

        if (UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(SpawnedHitbox))
        {
            Capsule->SetCapsuleHalfHeight(Params.HalfHeight);
            Capsule->SetCapsuleRadius(Params.Radius);
            return;
        }

        ensureNoEntry(); // Is the SpawnedHitbox type not supported yet? We should have covered it by this point.
    }
}

void FSpawnedHitbox::Destroy()
{
    if (SpawnedHitbox)
    {
        SpawnedHitbox->DestroyComponent(false /* bPromoteChildren */);
        SpawnedHitbox = nullptr;
    }

    OnHitboxOverlapDelegate.Clear();
    HitActors.Reset(); // We will be GC'd soon after this anyways
}

void FSpawnedHitbox::GenerateDamageEvent(AActor* Instigator, UGameplaySystemComponent* InstigatorGameplaySystem)
{
    if (Instigator && InstigatorGameplaySystem)
    {
        const UDamageCalculation* DamageCalculation = UDamageCalculation::GetFromClass(Params.HitboxAsset->DamageCalculationClass);
        PendingEvent = DamageCalculation->GenerateEvent(Instigator, InstigatorGameplaySystem, Params.EventFlags, Params.Locals);
        PendingEvent.EffectsOnTarget.Append(Params.EffectsOnTarget);
    }
}

UHitboxSourceComponent::UHitboxSourceComponent()
{

}

void UHitboxSourceComponent::BeginPlay()
{
    Super::BeginPlay();
}

UShapeComponent* UHitboxSourceComponent::CreateHitbox_Sphere(AActor* Actor, const FHitboxParams& Params)
{
    check(Actor);

    UHitboxAsset* HitboxAsset = Params.HitboxAsset;
    check(HitboxAsset);

    USphereComponent* SphereComponent = Cast<USphereComponent>(Actor->AddComponentByClass(USphereComponent::StaticClass(), true /* bManualAttachment */, Params.Socket.SocketOffset, false /* bDeferredFinish */));

    ensure(AttachHitbox(Params, SphereComponent, Actor));

    SphereComponent->SetSphereRadius(Params.Radius, true);

    return SphereComponent;
}

UShapeComponent* UHitboxSourceComponent::CreateHitbox_Capsule(AActor* Actor, const FHitboxParams& Params)
{
    check(Actor);

    UHitboxAsset* HitboxAsset = Params.HitboxAsset;
    check(HitboxAsset);

    UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(Actor->AddComponentByClass(UCapsuleComponent::StaticClass(), true /* bManualAttachment */, FTransform::Identity, false /* bDeferredFinish */));

    ensure(AttachHitbox(Params, CapsuleComponent, Actor));

    CapsuleComponent->SetCapsuleRadius(Params.Radius, true);
    CapsuleComponent->SetCapsuleHalfHeight(Params.HalfHeight, true);

    return CapsuleComponent;
}

USkeletalMeshComponent* UHitboxSourceComponent::GetTargetMeshFromActor(AActor* Actor, ETargetMesh TargetMesh, int32 Identifier)
{
    USkeletalMeshComponent* OutMesh = nullptr;

    switch (TargetMesh)
    {
        case ETargetMesh::ETM_None:
        {
            break;
        }

        case ETargetMesh::ETM_Weapon:
        {
            if (Actor->Implements<UMeshAccessorInterface>())
            {
                if (UWeaponComponent* Weapon = IMeshAccessorInterface::Execute_GetWeapon(Actor, Identifier))
                {
                    OutMesh = Weapon->GetActiveSkeletalMesh();
                    break;
                }
            }

            // This search won't usually run in-game, since all Characters should implement MeshAccessorInterface.
            // It's needed for the AnimationPreview Actor to find the correct injected components.

            TArray<UWeaponComponent*> Weapons;
            Actor->GetComponents<UWeaponComponent>(Weapons, false /* bIncludeFromChildActors */);

            for (auto Weapon : Weapons)
            {
                if (Weapon->GetIdentifier() == Identifier)
                {
                    OutMesh = Weapon->GetActiveSkeletalMesh();
                    break;
                }
            }

            break;
        }
        case ETargetMesh::ETM_SkeletalMesh:
        {
            if (Actor->Implements<UMeshAccessorInterface>())
            {
                if (USkeletalMeshComponent* SkeletalMesh = IMeshAccessorInterface::Execute_GetSkeletalMesh(Actor))
                {
                    OutMesh = SkeletalMesh;
                    break;
                }
            }

            // This search won't usually run in-game, since all Characters should implement MeshAccessorInterface.
            // It's needed for the AnimationPreview Actor to find the correct injected components.
            TArray<UWeaponComponent*> SkeletalMeshes;
            Actor->GetComponents<UWeaponComponent>(SkeletalMeshes, false /* bIncludeFromChildActors */);

            for (auto SkeletalMesh : SkeletalMeshes)
            {
                if (!SkeletalMesh->IsA<UWeaponComponent>())
                {
                    OutMesh = SkeletalMesh;
                    break;
                }
            }

            break;
        }
    }

    return OutMesh;
}

bool UHitboxSourceComponent::AttachHitbox(const FHitboxParams& Params, UShapeComponent* Hitbox, AActor* Actor)
{
    EAttachmentRule DefaultRule = EAttachmentRule::SnapToTarget;
    FAttachmentTransformRules AttachRules = { DefaultRule, false /* bInWeldSimulatedBodies */ };

    // Make sure we aren't already attached
    if (Hitbox->GetAttachParent())
    {
        FDetachmentTransformRules DetachRules = { AttachRules , false /* bInCallModify */ };

        Hitbox->DetachFromComponent(DetachRules);
    }

    USceneComponent* TargetComponent = GetTargetMeshFromActor(Actor, Params.Socket.Target);
    if (!TargetComponent)
    {
        TargetComponent = Actor->GetRootComponent();
    }

    ensure(TargetComponent);

    const bool bAttached = Hitbox->AttachToComponent(TargetComponent, AttachRules, Params.Socket.SocketName);
    Hitbox->SetRelativeTransform(Params.Socket.SocketOffset, true /* bSweep */);
    return bAttached;
}

bool UHitboxSourceComponent::OnSerialize(FSaveGameArchive& Archive, bool bIsLoading)
{
    const bool bSuccess = Archive.SerializeField(TEXT("HitboxSourceComponent"), [&](FStructuredArchive::FSlot Slot)
                                                {
                                                    SerializeScriptProperties(Slot);

                                                    int32 Count;

                                                    if (SAVING)
                                                    {
                                                        Count = AppliedModifiers.Num();
                                                    }

                                                    FStructuredArchiveArray ModifierArray = Slot.EnterArray(Count);

                                                    for (int32 Index = 0; Index < Count; Index++)
                                                    {
                                                        FSoftClassPath Class;

                                                        if (SAVING)
                                                        {
                                                            Class = AppliedModifiers[Index].GetClass();
                                                        }

                                                        ModifierArray.EnterElement() << Class;

                                                        if (LOADING)
                                                        {
                                                            UClass* ModifierClass = Class.TryLoadClass<UDamageEventModifier>();
                                                            if (ModifierClass)
                                                            {
                                                                AddDamageEventModifier(ModifierClass);
                                                            }
                                                            else
                                                            {
                                                                UE_LOG(VSLog, Error, TEXT("UHitboxSourceComponent: Couldn't resolve serialized DamageEventModifier class!"))
                                                            }
                                                        }
                                                    }
                                                });

    return bSuccess;
}

void UHitboxSourceComponent::SetInstigator(AActor* InInstigator)
{
    if (InInstigator)
    {
        OverrideInstigator = InInstigator;
    }
}

FSpawnedHitbox* UHitboxSourceComponent::GetHitboxFromHandle(const FSpawnedHitboxHandle& Handle)
{
    if (FSpawnedHitbox* SpawnedHitbox = SpawnedHitboxes.Find(Handle))
    {
        return SpawnedHitbox;
    }

    return nullptr;
}

UShapeComponent* UHitboxSourceComponent::GetInstanceFromHandle(const FSpawnedHitboxHandle& Handle)
{
    if (FSpawnedHitbox* SpawnedHitbox = SpawnedHitboxes.Find(Handle))
    {
        return SpawnedHitbox->SpawnedHitbox;
    }

    return nullptr;
}

FSpawnedHitboxHandle UHitboxSourceComponent::CreateHitbox(AActor* Actor, const FHitboxParams& Params, FOnHitboxOverlapSignature OverlapDelegate)
{
    return CreateHitbox_Internal(Actor, Params, OverlapDelegate);
}

bool UHitboxSourceComponent::DestroyHitbox(const FSpawnedHitboxHandle& Handle)
{
    FSpawnedHitbox* Hitbox = GetHitboxFromHandle(Handle);
    if (Hitbox)
    {
        Hitbox->Destroy();

        int32 Count = SpawnedHitboxes.Remove(Handle);
        ensure(Count == 1);

        return true;
    }

    return false;
}

void UHitboxSourceComponent::OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    FSpawnedHitboxHandle Handle = GetHandleFromPrimitive(OverlappedComponent);

    if (Handle.IsValid())
    {
        OverlappingHitboxes.Add(Handle);
        ProcessOverlappedHitbox(Handle, OtherActor, OtherComp, SweepResult);
    }
}

void UHitboxSourceComponent::OnHitboxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    FSpawnedHitboxHandle Handle = GetHandleFromPrimitive(OverlappedComponent);

    if (Handle.IsValid())
    {
        OverlappingHitboxes.Remove(Handle);
    }
}

AActor* UHitboxSourceComponent::GetInstigator() const
{
    if (AActor* OverrideInstigatorPtr = OverrideInstigator.Get())
    {
        return OverrideInstigatorPtr;
    }

    return GetOwner();
}

UGameplaySystemComponent* UHitboxSourceComponent::GetGameplaySystem()
{
    if (UGameplaySystemComponent* GameplaySystemPtr = GameplaySystem.Get())
    {
        return GameplaySystemPtr;
    }

    if (GetOwner()->Implements<UGameplaySystemOwnerInterface>())
    {
        UGameplaySystemComponent* GameplaySystemPtr = IGameplaySystemOwnerInterface::Execute_GetGameplaySystemComponent(GetOwner());
        GameplaySystem = MakeWeakObjectPtr(GameplaySystemPtr);

        return GameplaySystemPtr;
    }

    return nullptr;
}

UDamageEventModifier* UHitboxSourceComponent::AddDamageEventModifier(TSubclassOf<UDamageEventModifier> ModifierClass)
{
    if (ModifierClass)
    {
        UDamageEventModifier* NewModifier = NewObject<UDamageEventModifier>(this, ModifierClass);

        AppliedModifiers.Add(NewModifier);

        NewModifier->AddModifier(*this);

        return NewModifier;
    }

    return nullptr;
}

bool UHitboxSourceComponent::RemoveDamageEventModifierByType(TSubclassOf<UDamageEventModifier> ModifierClass)
{
    if (UDamageEventModifier* Modifier = GetDamageEventModifierByType(ModifierClass))
    {
        return RemoveDamageEventModifier(Modifier);
    }

    return false;
}

bool UHitboxSourceComponent::RemoveDamageEventModifier(UDamageEventModifier* Modifier)
{
    if (Modifier)
    {
        Modifier->RemoveModifier();

        const int32 Count = AppliedModifiers.Remove(Modifier);

        ensure(Count >= 1);

        return true;
    }

    return false;
}

UDamageEventModifier* UHitboxSourceComponent::GetDamageEventModifierByType(TSubclassOf<UDamageEventModifier> ModifierClass)
{
    for (const auto& Modifier : AppliedModifiers)
    {
        if (Modifier->IsA(ModifierClass))
        {
            return Modifier;
        }
    }

    return nullptr;
}

FAADamageEvent UHitboxSourceComponent::ApplyDamageEventModifiers(const FSpawnedHitbox* Hitbox)
{
    if (Hitbox)
    {
        FAADamageEvent OutModifiedEvent = Hitbox->PendingEvent;

        GetDefaultModifier()->TryApplyDamageEventModifier(OutModifiedEvent, Hitbox->Params);

        for (const auto& Modifier : AppliedModifiers)
        {
            Modifier->TryApplyDamageEventModifier(OutModifiedEvent, Hitbox->Params);
        }

        return OutModifiedEvent;
    }

    return FAADamageEvent();
}

FSpawnedHitboxHandle UHitboxSourceComponent::CreateHitbox_Internal(AActor* Actor, const FHitboxParams& Params, FOnHitboxOverlapSignature OverlapDelegate)
{
    UHitboxAsset* HitboxAsset = Params.HitboxAsset;
    if (!HitboxAsset)
    {
        return FSpawnedHitboxHandle();
    }

    FSpawnedHitboxHandle NewHandle = FSpawnedHitboxHandle::CreateNew();

    UShapeComponent* Hitbox = nullptr;

    if (Params.HalfHeight == 0.0f)
    {
        Hitbox = CreateHitbox_Sphere(Actor, Params);

    }
    else
    {
        Hitbox = CreateHitbox_Capsule(Actor, Params);
    }

    check(Hitbox);

    Hitbox->OnComponentBeginOverlap.AddUniqueDynamic(this, &UHitboxSourceComponent::OnHitboxBeginOverlap);
    Hitbox->OnComponentEndOverlap.AddUniqueDynamic(this, &UHitboxSourceComponent::OnHitboxEndOverlap);

    Hitbox->SetCollisionObjectType(CollisionChannel);

    // TODO: Improve the syntax used here to make it more clear what we are doing
    FSpawnedHitbox SpawnedHitbox = { Params, Hitbox, OverlapDelegate };

    SpawnedHitbox.GenerateDamageEvent(GetInstigator(), GetGameplaySystem());
    OnGenerateDamageEventDelegate.Broadcast(SpawnedHitbox.PendingEvent, SpawnedHitbox.Params);

    SpawnedHitboxes.Add(NewHandle, SpawnedHitbox);

    // Check for initial overlaps, since BeginOverlap / EndOverlap aren't triggered if the hitbox spawns in a collider
    TArray<FHitResult> HitResults;
    GetHitboxOverlaps(NewHandle, HitResults);

    for (const auto& HitResult : HitResults)
    {
        ProcessOverlappedHitbox(NewHandle, HitResult.GetActor(), HitResult.GetComponent(), HitResult);
    }

    return NewHandle;
}

void UHitboxSourceComponent::ProcessOverlappedHitbox(const FSpawnedHitboxHandle& OverlappedHandle, AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& SweepResult)
{
    FSpawnedHitbox* SpawnedHitbox = GetHitboxFromHandle(OverlappedHandle);
    check(SpawnedHitbox);

    // This can be hit in editor, if the overlap triggers while switching window or moving the Animation Preview timeline.
    if (!OtherActor)
    {
        return;
    }

    // Already hit this trace
    if (SpawnedHitbox->HitActors.Contains(OtherActor))
    {
        return;
    }

    if (!OtherActor->Implements<UReceiveDamageEventInterface>())
    {
        return;
    }

    if (IReceiveDamageEventInterface::Execute_IsDead(OtherActor))
    {
        return;
    }

    const bool bSameTeam = UGameplayUtilityBlueprintLibrary::AreSameTeam(GetInstigator(), OtherActor);
    if (bSameTeam)
    {
        return;
    }

    const FHitboxParams& Params = SpawnedHitbox->Params;
    UHitboxAsset* Asset = Params.HitboxAsset;
    if (!Asset)
    {
        return;
    }

    FHitResult DetailedResult = SweepResult;
    
    // This looks redundant, but we only get trivial properties from the ComponentBeginOverlap event and need to do this sweep to get important data
    // such as normals, hit location and penetration depth.
    {
        TArray<FHitResult> HitResults;
        GetHitboxOverlaps(OverlappedHandle, HitResults);

        bool bFoundComp = false;
        for (const auto& Hit : HitResults)
        {
            if (Hit.GetComponent() == OtherComp)
            {
                DetailedResult = Hit;
                bFoundComp = true;
                break;
            }
        }

        // In the case that we do not get the required data, inject some data points that we can roughly predict.
        if (not bFoundComp)
        {
            // We approximate the location to the midpoint between the hitbox and the hit component.
            const FVector HitboxLocation = SpawnedHitbox->SpawnedHitbox->GetComponentLocation();
            FVector Difference = HitboxLocation - OtherComp->GetComponentLocation();
            DetailedResult.Location = HitboxLocation + Difference * 0.5f;
        }
    }

    // Marshall in any remaining data into the DamageEvent before applying it

    FAADamageEvent ModifiedEvent = ApplyDamageEventModifiers(SpawnedHitbox);

    const UDamageCalculation* DamageCalculation = UDamageCalculation::GetFromClass(Asset->DamageCalculationClass);

    DamageCalculation->ApplyCollisionEvent(ModifiedEvent, DetailedResult, SpawnedHitbox->SpawnedHitbox);

    ModifiedEvent.ContextTags.AppendTags(SpawnedHitbox->Params.HitboxType);

    TOptional<FAAResolvedDamageEvent> EventResult = DamageCalculation->ApplyDamageEvent(OtherActor, ModifiedEvent);
    
    if (EventResult.IsSet())
    {
        SpawnedHitbox->HitActors.Emplace(OtherActor);
    
        SpawnedHitbox->OnHitboxOverlapDelegate.ExecuteIfBound(EventResult.GetValue());

        OnHitResolvedDelegate.Broadcast(EventResult.GetValue());
    }
}

FSpawnedHitboxHandle UHitboxSourceComponent::GetHandleFromPrimitive(UPrimitiveComponent* ComponentToFind)
{
    for (const auto& [Handle, SpawnedHitbox] : SpawnedHitboxes)
    {
        if (SpawnedHitbox.SpawnedHitbox == ComponentToFind)
        {
            return Handle;
        }
    }

    return FSpawnedHitboxHandle();
}

void UHitboxSourceComponent::GetHitboxOverlaps(const FSpawnedHitboxHandle& Handle, TArray<FHitResult>& HitResults)
{
    FSpawnedHitbox* SpawnedHitbox = GetHitboxFromHandle(Handle);

    if (!SpawnedHitbox)
    {
        return;
    }

    FComponentQueryParams QueryParams = {};
    UPrimitiveComponent* HitPrimitive = SpawnedHitbox->SpawnedHitbox;
    const FVector SweepLocation = HitPrimitive->GetComponentLocation();
    GetWorld()->ComponentSweepMulti(HitResults, HitPrimitive, SweepLocation, SweepLocation, HitPrimitive->GetComponentRotation(), QueryParams);
}

TSubclassOf<UDamageEventModifier> UHitboxSourceComponent::GetDefaultModifierClass() const
{
    return DefaultModifierClass ? DefaultModifierClass : TSubclassOf<UDamageEventModifier>(UDamageEventModifier::StaticClass());
}

UDamageEventModifier* UHitboxSourceComponent::GetDefaultModifier()
{
    if (not DefaultModifier)
    {
        DefaultModifier = NewObject<UDamageEventModifier>(this, GetDefaultModifierClass());

        DefaultModifier->AddModifier(*this);

        check(DefaultModifier);
    }

    return DefaultModifier;
}

