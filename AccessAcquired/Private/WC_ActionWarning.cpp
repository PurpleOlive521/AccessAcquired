// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "WC_ActionWarning.h"
#include "ActionWarningWidget.h"
#include "ModeComponent.h"
#include "HitboxSourceComponent.h"
#include "AnimNotifyState_SpawnHitbox.h"

FTrackedMontage::FTrackedMontage(UAnimMontage* Montage)
{
	AnimMontage = Montage;
}

uint32 GetTypeHash(const FTrackedMontage& InTrackedMontage)
{
	return GetTypeHash(InTrackedMontage.AnimMontage);
}

UWC_ActionWarning::UWC_ActionWarning(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UWC_ActionWarning::BeginPlay()
{
	Super::BeginPlay();

	ModeComponent = MakeWeakObjectPtr(GetOwner()->GetComponentByClass<UModeComponent>());

	if (USkeletalMeshComponent* SkeletalMesh = GetOwner()->GetComponentByClass<USkeletalMeshComponent>())
	{
		if (UAnimInstance* AnimInstancePtr = SkeletalMesh->GetAnimInstance())
		{
			AnimInstancePtr->OnMontageStarted.AddUniqueDynamic(this, &UWC_ActionWarning::OnMontageStarted);
			AnimInstancePtr->OnMontageEnded.AddUniqueDynamic(this, &UWC_ActionWarning::OnMontageEnded);

			AnimInstance = MakeWeakObjectPtr(AnimInstancePtr);
		}
	}
}

void UWC_ActionWarning::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UAnimInstance* AnimInstancePtr = AnimInstance.Get())
	{
		for (auto& TrackedMontage : TrackedMontages)
		{
			ProcessTrackedMontage(AnimInstancePtr, TrackedMontage);
		}
	}
}

void UWC_ActionWarning::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UAnimInstance* AnimInstancePtr = AnimInstance.Get())
	{
		AnimInstancePtr->OnMontageStarted.RemoveAll(this);
		AnimInstancePtr->OnMontageEnded.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UWC_ActionWarning::ActivateActionWarning(FAADamageEventFlags Flags)
{
	EDamageType Mode = EDamageType::EDT_Normal;

	if (UModeComponent* ModeComponentPtr = ModeComponent.Get())
	{
		Mode = ModeComponentPtr->GetMode();
	}

	GetWidgetInstance()->K2_PlayActionWarning(Mode, Flags.bIsUnparryable);
}

void UWC_ActionWarning::StopActionWarning()
{
	GetWidgetInstance()->K2_StopActionWarning();
}

UActionWarningWidget* UWC_ActionWarning::GetWidgetInstance()
{
	if (!WidgetInstance)
	{
		WidgetInstance = Cast<UActionWarningWidget>(GetWidget());
	}

	return WidgetInstance;
}

void UWC_ActionWarning::OnMontageStarted(UAnimMontage* Montage)
{
	if (UAnimInstance* AnimInstancePtr = AnimInstance.Get())
	{
		FTrackedMontage TrackedMontage = FTrackedMontage(Montage);

		// Find all SpawnHitbox notifies in the Montage and track their start time
		for (const FAnimNotifyEvent& Notify : Montage->Notifies)
		{
			const UAnimNotifyState_SpawnHitbox* HitboxNotify = Notify.NotifyStateClass ? Cast<UAnimNotifyState_SpawnHitbox>(Notify.NotifyStateClass) : nullptr;
			if (HitboxNotify)
			{
				const float StartTime = Notify.GetTriggerTime();

				FTriggerInfo TriggerInfo = FTriggerInfo( HitboxNotify, StartTime, HitboxNotify->Params.EventFlags /* InAdditionalFlags */);
				TrackedMontage.Triggers.Add(TriggerInfo);
			}
		}

		TrackedMontages.Add(TrackedMontage);
	}

	// Check immediately
}

void UWC_ActionWarning::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	TrackedMontages.Remove(FTrackedMontage(Montage));
}

void UWC_ActionWarning::ProcessTrackedMontage(UAnimInstance* AnimInstancePtr, FTrackedMontage& TrackedMontage)
{
	if (FAnimMontageInstance* ActiveMontage = AnimInstancePtr->GetInstanceForMontage(TrackedMontage.AnimMontage))
	{
		const float ActorTimeDilation = GetOwner()->GetActorTimeDilation();

		const float CurrentTime = ActiveMontage->GetPosition();

		const float PlayRate = ActiveMontage->GetPlayRate();
		const float AbsolutePlayRate = PlayRate * ActorTimeDilation;

		ResolveActivatedTriggers(TrackedMontage, AbsolutePlayRate, CurrentTime);
	}
}

bool UWC_ActionWarning::ShouldTrigger(const FTriggerInfo& Trigger, float RealTimeUntilTrigger) const
{
	if (Trigger.AdditionalFlags.bIsUnparryable)
	{
		return RealTimeUntilTrigger <= TriggerTime + UnparryableAdditionalTriggerTime;
	}

	return RealTimeUntilTrigger <= TriggerTime;
}

void UWC_ActionWarning::ResolveActivatedTriggers(FTrackedMontage& TrackedMontage, float AbsolutePlayRate, float Position)
{
	for (int32 Index = 0; Index < TrackedMontage.Triggers.Num(); Index++)
	{
		const FTriggerInfo& Trigger = TrackedMontage.Triggers[Index];
		const float RealTimeUntilTrigger = (Trigger.TriggerTime - Position) * AbsolutePlayRate;
		if (ShouldTrigger(Trigger, RealTimeUntilTrigger))
		{
			check(Trigger.Notify.IsValid());

			ActivateActionWarning(Trigger.Notify->Params.EventFlags);

			// The trigger is activated and no longer needed. Remove and decrement before continuing the check.
			TrackedMontage.Triggers.RemoveAt(Index);
			Index--;
		}
	}
}

FTriggerInfo::FTriggerInfo(const UAnimNotifyState_SpawnHitbox* InNotify, float InTriggerTime, FAADamageEventFlags InAdditionalFlags)
{
	check(InNotify);

	Notify = MakeWeakObjectPtr(InNotify);
	TriggerTime = InTriggerTime;
	AdditionalFlags = InAdditionalFlags;
}
