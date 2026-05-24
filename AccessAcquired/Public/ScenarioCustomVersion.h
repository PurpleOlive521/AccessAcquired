// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"

struct VERTICALSLICE_API FScenarioCustomVersion
{
	enum Type
	{
		// Before any version changes or custom properties were added
		BeforeCustomVersionWasAdded = 0,

		// Serialize base properties, like vacant participant Ids and the given Id.
		InitialCustomProperties = 1,



		// --- Add new versions above this line
		VersionPlusOne,
		LatestVersion = VersionPlusOne - 1
	};

	// The GUID for this custom version number
	const static FGuid GUID;

private:
	FScenarioCustomVersion() = default;
};
