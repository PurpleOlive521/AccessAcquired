// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Level/GateComponent.h"
#include "LatentCurveEvaluator.h"
#include "LatentCurveEvaluatorBlueprintLibrary.h"

UGateComponent::UGateComponent(const FObjectInitializer& ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	SetMobility(EComponentMobility::Movable);
}

void UGateComponent::OnRegister()
{
	Super::OnRegister();

	MovingDirection.Normalize();
}

void UGateComponent::PostEditImport()
{
	Super::PostEditImport();

	AnimationEvaluator = nullptr;
}

void UGateComponent::SetGateState(EGateState InState)
{
	if (State == InState)
	{
		return;
	}

	SetGateState_Internal(InState);

	switch (State)
	{
		case EGateState::EGS_Open:
		{
			AnimationEvaluator->SetPlayDirection(EPlayDirection::EPD_Forward);
			AnimationEvaluator->Play();
			break;
		}
		case EGateState::EGS_Closed:
		{
			AnimationEvaluator->SetPlayDirection(EPlayDirection::EPD_Backward);

			AnimationEvaluator->Play();
			break;
		}
	}
}

void UGateComponent::SetGateState_Instantaneous(EGateState InState)
{
	if (State == InState)
	{
		return;
	}

	SetGateState_Internal(InState);

	float Evaluate = 0.0f;

	// We make sure to also set the time we evaluate at, so the evaluator starts counting from the right point.
	switch (State)
	{
	case EGateState::EGS_Closed:
	{
		AnimationEvaluator->SetEvaluatedTime(0.0f);
		Evaluate = AnimationEvaluator->EvaluateCurve();
		OnAnimationUpdated(Evaluate);
		break;
	}
	case EGateState::EGS_Open:
	{
		AnimationEvaluator->SetEvaluatedTime(AnimationEvaluator->GetLastKey());
		Evaluate = AnimationEvaluator->EvaluateCurve();
		OnAnimationUpdated(Evaluate);
		break;
	}
	}
}

void UGateComponent::SetGateState_Internal(EGateState InState)
{
	if (State == InState)
	{
		return;
	}

	State = InState;

	// Set Evaluator
	if (!AnimationEvaluator)
	{

		FLatentCurveEvaluatorParams Params;
		Params.Curve = AnimationCurve;

		Params.OnEvaluateDelegate.BindDynamic(this, &UGateComponent::OnAnimationUpdated);

		FOnFinishedSignature OnFinishedDelegate;
		Params.OnFinishedDelegate.BindDynamic(this, &UGateComponent::OnAnimationFinished);
		

		AnimationEvaluator = ULatentCurveEvaluatorBlueprintLibrary::CreateLatentCurveEvaluator(this, Params);
	}
}

void UGateComponent::OnAnimationUpdated(float EvaluatedFloat)
{
	const FVector Destination = MovingDirection * EvaluatedFloat * TravelDistance + StartPosition;
	SetRelativeLocation(Destination);
}

void UGateComponent::OnAnimationFinished()
{
}
