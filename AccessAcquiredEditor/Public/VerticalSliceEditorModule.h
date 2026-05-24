// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FVerticalSliceEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};