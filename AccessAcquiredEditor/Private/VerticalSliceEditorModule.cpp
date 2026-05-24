// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "VerticalSliceEditorModule.h"

#include "EnemySelectorCustomization.h"

IMPLEMENT_GAME_MODULE(FVerticalSliceEditorModule, VerticalSliceEditor);

void FVerticalSliceEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("EnemySelector", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FEnemySelectorCustomization::MakeInstance));
}

void FVerticalSliceEditorModule::ShutdownModule()
{
	// Unregister customizations
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout("EnemySelector");
	}
}