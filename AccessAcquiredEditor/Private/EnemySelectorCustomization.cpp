// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "EnemySelectorCustomization.h"
#include "EditorCustomizationHelpers.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyUtilities.h"

TSharedRef<IPropertyTypeCustomization> FEnemySelectorCustomization::MakeInstance()
{
	return MakeShared<FEnemySelectorCustomization>();
}

void FEnemySelectorCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FEnemySelectorCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> InStructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}

void FEnemySelectorCustomization::OnPropertyChanged(TSharedPtr<IPropertyUtilities> Utils)
{
	if (Utils)
	{
		Utils->RequestForceRefresh();
	}
}
