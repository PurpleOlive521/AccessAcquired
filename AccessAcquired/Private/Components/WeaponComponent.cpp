// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "WeaponComponent.h"
#include "DevCommons.h"
#include "GameplayUtilityBlueprintLibrary.h"

#include "GameplaySystemOwnerInterface.h"
#include "GameplaySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "WeaponAnimInstance.h"
#include "MeshAccessorInterface.h"
#include "UObject/UnrealType.h"

UWeaponComponent::UWeaponComponent(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWeaponComponent::OnRegister()
{
	Super::OnRegister();

	PerformAutomaticAttachment();
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
}

#if WITH_EDITORONLY_DATA

void UWeaponComponent::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	UndoAutomaticAttachment();
}

void UWeaponComponent::PreEditChange(FEditPropertyChain& PropertyAboutToChange)
{
	UObject::PreEditChange(PropertyAboutToChange); // Super typedef does not like the overloaded function

	UndoAutomaticAttachment();
}

void UWeaponComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Reattach in case we modified AttachmentRules
	PerformAutomaticAttachment();
}
#endif //WITH_EDITORONLY_DATA

UWeaponComponent* UWeaponComponent::GetWeaponFromActor(AActor* Actor, int32 Identifier)
{
	check(Actor);

	if (Actor->Implements<UMeshAccessorInterface>())
	{
		UWeaponComponent* Weapon = IMeshAccessorInterface::Execute_GetWeapon(Actor, Identifier);
		if (Weapon)
		{
			return Weapon;
		}
	}

	TArray<UWeaponComponent*> WeaponComponents;
	Actor->GetComponents<UWeaponComponent>(WeaponComponents);

	for (auto WeaponComponent : WeaponComponents)
	{
		if (WeaponComponent->GetIdentifier() == Identifier)
		{
			return WeaponComponent;
		}
	}

	return nullptr;
}

void UWeaponComponent::OnHit(const FAAResolvedDamageEvent& ResolvedEvent)
{
	// Event was either not received or ignored by gameplay logic
	if (ResolvedEvent.EventStates.bWasEventIgnored)
	{
		return;
	}

	K2_OnHit(ResolvedEvent);
}

USkeletalMeshComponent* UWeaponComponent::GetActiveSkeletalMesh()
{
	if (bUseOverrideSkeletalMesh)
	{
		if (USkeletalMeshComponent* OverrideSkeletalMeshPtr = OverrideSkeletalMesh.Get())
		{
			return OverrideSkeletalMeshPtr;
		}

		// We don't have a OverrideSkeletalMesh, use the first non-WeaponComponent we find
		if (AActor* Owner = GetOwner())
		{
			TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
			Owner->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

			for (auto SkeletalMeshComponent : SkeletalMeshComponents)
			{
				if (SkeletalMeshComponent->IsA<UWeaponComponent>())
				{
					continue;
				}

				if (!SkeletalMeshComponent->ComponentHasTag(OverrideSkeletalMeshTag) && !SkeletalMeshComponent->ComponentHasTag(AnimationPreviewerMeshTag))
				{
					continue;
				}

				OverrideSkeletalMesh = SkeletalMeshComponent;
				return OverrideSkeletalMesh;
			}
		}
		else
		{
			return nullptr;
		}
	}

	return this;
}

void UWeaponComponent::EnableWeaponTrails()
{
	if (bHasEnabledTrails)
	{
		return;
	}

	bHasEnabledTrails = true;

	K2_EnableWeaponTrails();
}

void UWeaponComponent::DisableWeaponTrails()
{
	if (!bHasEnabledTrails)
	{
		return;
	}

	bHasEnabledTrails = false;

	K2_DisableWeaponTrails();
}

void UWeaponComponent::PlayEquipAnimation()
{
	if (bHasEquippedWeapon)
	{
		return;
	}

	bHasEquippedWeapon = true;

	UWeaponAnimInstance* WeaponAnimInstance = GetWeaponAnimInstance();
	if (!WeaponAnimInstance)
	{
		UE_LOG(VSLog, Error, TEXT("WeaponComponent: No WeaponAnimInstance-derived AnimInstance found!"));
		return;
	}
		
	WeaponAnimInstance->OnWeaponEquipped();
}

void UWeaponComponent::PlayUnequipAnimation()
{
	if (!bHasEquippedWeapon)
	{
		return;
	}

	bHasEquippedWeapon = false;

	UWeaponAnimInstance* WeaponAnimInstance = GetWeaponAnimInstance();
	if (!WeaponAnimInstance)
	{
		UE_LOG(VSLog, Error, TEXT("No WeaponAnimInstance-derived AnimInstance found on WeaponComponent!"));
		return;
	}

	WeaponAnimInstance->OnWeaponUnequipped();
}

void UWeaponComponent::PerformAutomaticAttachment()
{
	if (bDisableAutomaticAttachment)
	{
		return;
	}

	if (!AttachmentRules)
	{
		return;
	}

	USceneComponent* AttachmentTarget = nullptr;
	switch (AttachmentRules->PreferredRoot)
	{
	case EPreferredRoot::EPR_Manual:
	{
		AttachmentTarget = GetAttachParent();
		break;
	}
	case EPreferredRoot::EPR_Root:
	{
		if (AActor* Owner = GetOwner())
		{
			AttachmentTarget = Owner->GetRootComponent();
		}
		
		break;
	}
	case EPreferredRoot::EPR_SkeletalMesh:
	{
		AActor* Owner = GetOwner();
		if (!Owner)
		{
			return;
		}

		if (Owner->Implements<UMeshAccessorInterface>())
		{
			AttachmentTarget = IMeshAccessorInterface::Execute_GetSkeletalMesh(GetOwner());
		}

		if (!AttachmentTarget)
		{
			if (USceneComponent* Root = Owner->GetRootComponent())
			{
				TArray<USceneComponent*> Children;
				Root->GetChildrenComponents(false /* bIncludeAllDescendants */, Children);
				Children.Insert(Root, 0); // We wan't to query root itself first

				for (auto ChildComp : Children)
				{
					if (ChildComp == this)
					{
						continue;
					}

					if (ChildComp->IsA<USkeletalMeshComponent>() && !ChildComp->IsA<UWeaponComponent>())
					{
						AttachmentTarget = ChildComp;
						break;
					}
				}
			}
		}
	}
	}

	if (AttachmentTarget)
	{
		EAttachmentRule DefaultRule = EAttachmentRule::SnapToTarget;
		FAttachmentTransformRules Rules = { DefaultRule, false /* bInWeldSimulatedBodies */ };
		const bool bAttached = AttachToComponent(AttachmentTarget, Rules, AttachmentRules->Socket);
		SetRelativeTransform(AttachmentRules->SocketOffset, false /* bSweep */);

		if (!bAttached)
		{
			UE_LOG(VSLog, Warning, TEXT("WeaponComponent: Automatic attachment failed!"));
		}
	}
}

void UWeaponComponent::UndoAutomaticAttachment()
{
	if (!bDisableAutomaticAttachment)
	{
		EAttachmentRule DefaultRule = EAttachmentRule::SnapToTarget;
		FAttachmentTransformRules AttachRules = { DefaultRule, false /* bInWeldSimulatedBodies */ };
		FDetachmentTransformRules DetachRules = { AttachRules , false /* bInCallModify */ };
		DetachFromComponent(DetachRules);
	}
}

void UWeaponComponent::SetIdentifier(int32 InIdentifier)
{
	Identifier = InIdentifier;
}

int32 UWeaponComponent::GetIdentifier() const
{
	return Identifier;
}

UWeaponAnimInstance* UWeaponComponent::GetWeaponAnimInstance()
{
	return Cast<UWeaponAnimInstance>(GetAnimInstance());
}

