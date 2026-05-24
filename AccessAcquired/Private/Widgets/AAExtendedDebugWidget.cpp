// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/AAExtendedDebugWidget.h"
#include "GameplayUtilityBlueprintLibrary.h"
#include "DevelopmentTypes.h"
#include "EnemyAIControllerBase.h"
#include "GameplaySystemComponent.h"

using namespace DebugTypes;

void UAAExtendedDebugWidget::OnEnabledTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::OnEnabledTick(MyGeometry, InDeltaTime);

	K2_TickAIControllerDisplay(InDeltaTime);
}

void UAAExtendedDebugWidget::OnBoundToGameplaySystem(UGameplaySystemComponent* GameplaySystem)
{
	Super::OnBoundToGameplaySystem(GameplaySystem);

	BindToAIController();
}

FString UAAExtendedDebugWidget::GetAIControllerGenericDisplayInfo() const
{
	FString DisplayInfo = TextTag_Header + TEXT("Enemy AI Controller Debug Info:") + TextTag_End + ENDL;

	const bool bValidAIController = BoundAIController.IsValid();

	DisplayInfo += TEXT("Has EnemyAIController: ") + TextTag_Highlight + (bValidAIController ? FString(TEXT("Yes")) : FString(TEXT("No"))) + TextTag_End + ENDL;
	if (bValidAIController)
	{
		DisplayInfo += TEXT("	AIState: ") + TextTag_Highlight + UGameplayUtilityBlueprintLibrary::ConvertAIStateToDisplayName(BoundAIController->GetCurrentState()) + TextTag_End + ENDL;
	}

	return DisplayInfo;
}

FString UAAExtendedDebugWidget::GetAIControllerTargetDisplayInfo() const
{
	FString DisplayInfo = TextTag_Header + TEXT("Known Targets:") + TextTag_End + ENDL;

	if (AEnemyAIControllerBase* Controller = BoundAIController.Get())
	{
		for (const auto& KnownTarget : Controller->KnownTargets)
		{
			AActor* Target = KnownTarget.GetTarget();
			DisplayInfo += (Target ? Target->GetActorNameOrLabel() : TEXT("INVALID")) + FString::Printf(TEXT(" Priority %.1f"), KnownTarget.Priority) + ENDL;
		}
	}

	return DisplayInfo;
}

void UAAExtendedDebugWidget::BindToAIController()
{
	BoundAIController = nullptr;
	if (UGameplaySystemComponent* GameplaySystemPtr = BoundGameplaySystem.Get())
	{
		if (AActor* Owner = GameplaySystemPtr->GetOwner())
		{
			if (AEnemyAIControllerBase* Controller = Owner->GetInstigatorController<AEnemyAIControllerBase>())
			{
				BoundAIController = MakeWeakObjectPtr(Controller);
			}
		}
	}
}