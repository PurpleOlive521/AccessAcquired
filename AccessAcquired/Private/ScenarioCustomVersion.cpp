// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "ScenarioCustomVersion.h"
#include "Serialization/CustomVersion.h"

const FGuid FScenarioCustomVersion::GUID(0x587cc24f, 0x0e9f30ec, 0x6adfb48d, 0x5db96ce8);

FCustomVersionRegistration GRegisterScenarioCustomVersion(FScenarioCustomVersion::GUID, FScenarioCustomVersion::LatestVersion, TEXT("ScenarioVer"));
