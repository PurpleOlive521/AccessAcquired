// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "GameplayEvents/GE_SetExpression.h"
#include "MeshAccessorInterface.h"
#include "DevCommons.h"
#include "GameplayTagDefines.h"

UGE_SetExpression::UGE_SetExpression()
{
	bActorUnique = true;
	bReplaceOnUnique = true;
	bShareOwnerLifetime = true;

	DisplayName = "Set Expression Material";

	EventTags.AddTag(GAMEPLAYTAG_GameplayEvent_Types_Expression);
	EventTags.AddTag(GAMEPLAYTAG_GameplayEvent_Effects_Material);

	TickSource = ETickSource::ETS_SourceDeltaTime;
	InstancingPolicy = EEventInstancingPolicy::EEIP_Instanced;
}

void UGE_SetExpression::SetExpression()
{
	if (!ExpressionMaterial)
	{
		UE_LOG(VSLog, Warning, TEXT("No ExpressionMaterial set for GameplayEvent: %s!"), *DisplayName);
		return;
	}

	if (AActor* OwnerActor = GetOwnerAsActor())
	{
		if (OwnerActor->Implements<UMeshAccessorInterface>())
		{
			USkeletalMeshComponent* SkeletalMesh = IMeshAccessorInterface::Execute_GetSkeletalMesh(OwnerActor);
			if (!SkeletalMesh)
			{
				UE_LOG(VSLog, Error, TEXT("Tried to get SkeletalMesh from Actor Owner of GameplayEvent: %s, but it returned null!"), *DisplayName);
				return;
			}

			OriginalMaterial = SkeletalMesh->GetMaterial(MaterialIndex);
			SkeletalMesh->SetMaterial(MaterialIndex, ExpressionMaterial);
		}
		else
		{
			UE_LOG(VSLog, Error, TEXT("Can't get MeshAccessor interface of GameplayEvent: %s!"), *DisplayName);
			return;
		}

	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("Tried to get Mesh from non-actor owner of GameplayEvent: %s!"), *DisplayName);
	}
}

void UGE_SetExpression::RestoreExpression()
{
	if (!OriginalMaterial)
	{
		return;
	}

	if (AActor* OwnerActor = GetOwnerAsActor())
	{
		if (OwnerActor->Implements<UMeshAccessorInterface>())
		{
			USkeletalMeshComponent* SkeletalMesh = IMeshAccessorInterface::Execute_GetSkeletalMesh(OwnerActor);
			if (!SkeletalMesh)
			{
				UE_LOG(VSLog, Error, TEXT("Tried to get SkeletalMesh from Actor Owner of GameplayEvent: %s, but it returned null!"), *DisplayName);
				return;
			}
			
			SkeletalMesh->SetMaterial(MaterialIndex, OriginalMaterial);
		}
		else
		{
			UE_LOG(VSLog, Error, TEXT("Can't get MeshAccessor interface of GameplayEvent: %s!"), *DisplayName);
			return;
		}
	}
	else
	{
		UE_LOG(VSLog, Error, TEXT("Tried to get Mesh from non-actor owner of GameplayEvent: %s!"), *DisplayName);
	}
}

void UGE_SetExpression::TriggerEvent(const FGameplayEventActivationData& ActivationData)
{
	Super::TriggerEvent(ActivationData);

	SetExpression();
}

void UGE_SetExpression::EndEvent()
{
	if (bRevertExpression)
	{
		RestoreExpression();
	}

	Super::EndEvent();
}

void UGE_SetExpression::AbortEvent()
{
	Super::AbortEvent();

	FinishAbortWithEndEvent();
}