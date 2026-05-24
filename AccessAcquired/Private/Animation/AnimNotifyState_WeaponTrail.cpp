// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AnimNotifyState_WeaponTrail.h"
#include "WeaponComponent.h"

UAnimNotifyState_WeaponTrail::UAnimNotifyState_WeaponTrail()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = true;
#endif //WITH_EDITORONLY_DATA
}

void UAnimNotifyState_WeaponTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AActor* Owner = MeshComp->GetOwner();

	if (Owner)
	{
		UWeaponComponent* Weapon = UWeaponComponent::GetWeaponFromActor(Owner, WeaponIdentifier);
		if (Weapon)
		{
			Weapon->EnableWeaponTrails();
			WeaponComponent = MakeWeakObjectPtr(Weapon);
		}
	}
}

void UAnimNotifyState_WeaponTrail::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UAnimNotifyState_WeaponTrail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (UWeaponComponent* WeaponPtr = WeaponComponent.Get())
	{
		WeaponPtr->DisableWeaponTrails();
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
