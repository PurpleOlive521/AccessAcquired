// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "AI/BTService_IsTargetValid.h"
#include "AIController.h"
#include "NavigationSystem.h"

bool UBTService_IsTargetValid::IsPointReachable(AAIController* Controller, const FVector& Location)
{
	const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Controller->GetWorld());
	if (NavSys)
	{
		const ANavigationData* NavData = Controller ? NavSys->GetNavDataForProps(Controller->GetNavAgentPropertiesRef(), Controller->GetNavAgentLocation()) : NULL;
		if (NavData)
		{
			FSharedConstNavQueryFilter QueryFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, Controller, FilterClass);

			FVector Start = FVector{};

			if (APawn* Pawn = Controller->GetPawn())
			{
				Start = Pawn->GetActorLocation();
			}

			EPathFindingMode::Type TestMode = EPathFindingMode::Hierarchical;

			const bool bHasPath = NavSys->TestPathSync(FPathFindingQuery(Controller, *NavData, Start, Location, QueryFilter), TestMode);
			return bHasPath;
		}
	}

	return false;
}
