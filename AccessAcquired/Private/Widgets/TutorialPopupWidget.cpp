// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Widgets/TutorialPopupWidget.h"
#include "DevCommons.h"

FPrimaryAssetId UTutorialPopupContent::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("TutorialPopupItems", GetFName());
}

UObject* UTutorialPopupContent::GetImage() const
{
	return Image.LoadSynchronous();
}

FPrimaryAssetId UTutorialPopupCollection::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("TutorialPopupItems", GetFName());
}

UTutorialPopupContent* UTutorialPopupCollection::GetPopupContent(int32 Index)
{
	if (Index >= 0 && Index < TutorialPopupAssets.Num())
	{
		return TutorialPopupAssets[Index];
	}

	return nullptr;
}

bool UTutorialPopupCollection::IsValidIndex(int32 Index) const
{
	if (Index >= 0 && Index < TutorialPopupAssets.Num())
	{
		return true;
	}

	return false;
}

int32 UTutorialPopupCollection::GetPopupContentCount() const
{
	return TutorialPopupAssets.Num();
}

FTutorialPopupWidgetParams::FTutorialPopupWidgetParams(bool bInPauseWhenAdded, bool bInUnpauseWhenRemoved)
	: bPauseWhenAdded(bInPauseWhenAdded), bUnpauseWhenRemoved(bInUnpauseWhenRemoved)
{
}

void UTutorialPopupWidget::AssignContentAsset(UTutorialPopupCollection* Asset, const FTutorialPopupWidgetParams& InParams)
{
	if (Asset)
	{
		if (ContentAsset != Asset)
		{
			ContentAsset = Asset;

			DisplayedContentIndex = 0;

			Params = InParams;

			K2_OnSetParams(Params);

			if (ContentAsset->IsValidIndex(DisplayedContentIndex))
			{
				K2_OnContentChanged();
			}
			else
			{
				UE_LOG(VSLog, Error, TEXT("TutorialPopupWidget: Received TutorialPopupCollection asset without any content!"));
			}
		}
	}
}

void UTutorialPopupWidget::RequestNextPage()
{
	if (IsLastPage())
	{
		CloseWidget();
		return;
	}

	const int32 NextIndex = DisplayedContentIndex + 1;

	SetDisplayedContentIndex(NextIndex);
}

void UTutorialPopupWidget::RequestPreviousPage()
{
	const int32 NextIndex = DisplayedContentIndex - 1;

	SetDisplayedContentIndex(NextIndex);
}

UTutorialPopupContent* UTutorialPopupWidget::GetCurrentContent()
{
	if (ContentAsset)
	{
		return ContentAsset->GetPopupContent(DisplayedContentIndex);
	}

	return nullptr;
}

bool UTutorialPopupWidget::IsLastPage() const
{
	if (ContentAsset)
	{
		const int32 NextIndex = DisplayedContentIndex + 1;

		// We are the last page if the next index is invalid
		return not ContentAsset->IsValidIndex(NextIndex);
	}

	return true;
}

bool UTutorialPopupWidget::IsFirstPage() const
{
	if (ContentAsset)
	{
		const int32 NextIndex = DisplayedContentIndex - 1;

		// We are the last page if the previous index is invalid
		return not ContentAsset->IsValidIndex(NextIndex);
	}

	return true;
}

int32 UTutorialPopupWidget::GetPageCount() const
{
	if (ContentAsset)
	{
		return ContentAsset->GetPopupContentCount();
	}

	return 0;
}

void UTutorialPopupWidget::CloseWidget()
{
	OnCollectionFinishedDelegate.Broadcast(ContentAsset);
	OnCollectionFinishedDelegate.Clear();

	DeactivateWidget();
}

void UTutorialPopupWidget::SetDisplayedContentIndex(int32 NewIndex)
{
	if (DisplayedContentIndex != NewIndex)
	{
		if (ContentAsset)
		{
			if (ContentAsset->IsValidIndex(NewIndex))
			{
				DisplayedContentIndex = NewIndex;
				K2_OnContentChanged();
			}
		}
	}
}
