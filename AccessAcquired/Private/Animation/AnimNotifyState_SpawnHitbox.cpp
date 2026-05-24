// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AnimNotifyState_SpawnHitbox.h"
#include "WeaponComponent.h"
#include "MeshAccessorInterface.h"
#include "DevCommons.h"
#include "Components/ShapeComponent.h"

UAnimNotifyState_SpawnHitbox::UAnimNotifyState_SpawnHitbox()
{
#if WITH_EDITORONLY_DATA
    bShouldFireInEditor = true;
#endif //WITH_EDITORONLY_DATA
}

void UAnimNotifyState_SpawnHitbox::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) // Can be nullptr when switching the previewed animation
    {
        return;
    }

    UHitboxSourceComponent* HitboxSource = Owner->GetComponentByClass<UHitboxSourceComponent>();

    UWeaponComponent* Weapon = nullptr;

    Weapon = UWeaponComponent::GetWeaponFromActor(Owner, Params.Socket.MeshIdentifier);

#if WITH_EDITORONLY_DATA

    if (bResetDisplayWeapon)
    {
        ResetDisplayWeapon();
        bResetDisplayWeapon = false;
    }

    SkeletalMesh = MakeWeakObjectPtr(MeshComp);

    if (bIsPlayingInEditor)
    {
        // Inject a HitboxSourceComponent
        if (!HitboxSource)
        {
            HitboxSource = Cast<UHitboxSourceComponent>(Owner->AddComponentByClass(UHitboxSourceComponent::StaticClass(), false /* bManualAttachment */, FTransform::Identity, false /* bDeferredFinish */));
        }

        // Inject AnimationPreviewerMeshTag on the Mesh
        if (!MeshComp->ComponentHasTag(AnimationPreviewerMeshTag))
        {
            MeshComp->ComponentTags.Add(AnimationPreviewerMeshTag);
        }

        // Inject the placeholder WeaponComponent
        if (!Weapon && !WeaponComponent.IsValid())
        {
            if (UClass* DisplayWeaponClass = DisplayWeapon.LoadSynchronous())
            {
                Weapon = Cast<UWeaponComponent>(Owner->AddComponentByClass(DisplayWeaponClass, false /* bManualAttachment */, FTransform::Identity, false /* bDeferredFinish */));
                Weapon->SetIdentifier(Params.Socket.MeshIdentifier);
                Weapon->PlayEquipAnimation();
            }
        }
    }

#endif // WITH_EDITORONLY_DATA

    if (HitboxSource)
    {
        HitboxSourceComponent = MakeWeakObjectPtr(HitboxSource);
        WeaponComponent = MakeWeakObjectPtr(Weapon);

        // TODO: Tie this up better? We are now responsible for binding for the WeaponComponent which doesn't feel right.
        FOnHitboxOverlapSignature Delegate;
        if (Weapon && Params.Socket.Target == ETargetMesh::ETM_Weapon)
        {
            Delegate.BindDynamic(Weapon, &UWeaponComponent::OnHit);
        }

        Handle = HitboxSourceComponent->CreateHitbox(MeshComp->GetOwner(), Params, Delegate);
    }
}

void UAnimNotifyState_SpawnHitbox::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_SpawnHitbox::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (UHitboxSourceComponent* HitboxSourcePtr = HitboxSourceComponent.Get())
    {
        HitboxSourcePtr->DestroyHitbox(Handle);
    }

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}

#if WITH_EDITORONLY_DATA

void UAnimNotifyState_SpawnHitbox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    if (bIsPlayingInEditor)
    {
        if (Handle.IsValid() && HitboxSourceComponent.IsValid())
        {
            UHitboxSourceComponent* HitboxSourcePtr = HitboxSourceComponent.Get();

            FSpawnedHitbox* HitboxInstance = HitboxSourcePtr->GetHitboxFromHandle(Handle);

            if (HitboxInstance)
            {
                HitboxInstance->UpdateParams(Params);
            }
        }
    }
}
#endif //WITH_EDITORONLY_DATA

void UAnimNotifyState_SpawnHitbox::ResetDisplayWeapon()
{

#if WITH_EDITORONLY_DATA

    if (bIsPlayingInEditor)
    {
        if (UWeaponComponent* WeaponPtr = WeaponComponent.Get())
        {
            WeaponPtr->DestroyComponent(true /* bPromoteChildren */);
        }
    }

#endif // WITH_EDITORONLY_DATA

}

TArray<FName> UAnimNotifyState_SpawnHitbox::GetSocketNames() const
{
#if WITH_EDITORONLY_DATA
    // This will only ever run in the Animation Previewer, so we can't use the MeshAccessorInterface
    // Hence we need to find the components ourselves

    if (USkeletalMeshComponent* SkeletalMeshPtr = SkeletalMesh.Get())
    {
        const ETargetMesh Target = Params.Socket.Target;
        if (Target == ETargetMesh::ETM_SkeletalMesh)
        {
            return SkeletalMeshPtr->GetAllSocketNames();
        }

        if (Target == ETargetMesh::ETM_Weapon)
        {
            TArray<USceneComponent*> ChildrenComps;
            SkeletalMesh->GetChildrenComponents(true /* bIncludeAllDescendants */, ChildrenComps);

            for (auto ChildrenComp : ChildrenComps)
            {
                if (ChildrenComp->IsA<UWeaponComponent>())
                {
                    UWeaponComponent* Weapon = Cast<UWeaponComponent>(ChildrenComp);

                    if (Weapon->GetIdentifier() == Params.Socket.MeshIdentifier)
                    {
                        return Weapon->GetActiveSkeletalMesh()->GetAllSocketNames();
                    }
                }
            }
        }

    }

#endif //WITH_EDITORONLY_DATA

    return TArray<FName>();
}

