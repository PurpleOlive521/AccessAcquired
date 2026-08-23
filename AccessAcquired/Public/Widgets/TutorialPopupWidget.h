// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "Widgets/ExtendedCommonActivatableWidget.h"
#include "TutorialPopupWidget.generated.h"

/**
 * Content for a single TutorialPopup.
 * Multiple can be combined in a TutorialPopupCollection to form a sequence of content to display.
 */
UCLASS()
class VERTICALSLICE_API UTutorialPopupContent : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- Begin UPrimaryDataAsset Interface
	FPrimaryAssetId GetPrimaryAssetId() const override;
	// --- End UPrimaryDataAsset Interface

	// Can return nullptr if no image is selected for this asset.
	UFUNCTION(BlueprintCallable, Category = "TutorialPopupContent")
	UObject* GetImage() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TutorialPopupContent")
	FText SubtitleText;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (MultiLine = true), Category = "TutorialPopupContent")
	FText BodyText;

	// Matching FSlateBrush::ResourceObject implementation.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayThumbnail = "true", AllowedClasses = "/Script/Engine.Texture,/Script/Engine.MaterialInterface,/Script/Engine.SlateTextureAtlasInterface", DisallowedClasses = "/Script/MediaAssets.MediaTexture"), Category = "TutorialPopupContent")
	TSoftObjectPtr<UObject> Image = nullptr;
};

UCLASS()
class VERTICALSLICE_API UTutorialPopupCollection : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// --- Begin UPrimaryDataAsset Interface
	FPrimaryAssetId GetPrimaryAssetId() const override;
	// --- End UPrimaryDataAsset Interface

	// Returns nullptr if Index is not valid.
	UTutorialPopupContent* GetPopupContent(int32 Index);

	bool IsValidIndex(int32 Index) const;

	int32 GetPopupContentCount() const;

	// The title displayed for any TutorialPopupContent displayed through this asset.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TutorialPopupCollection")
	FText TitleText;

	// The order in the array matches the order they are displayed in.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "TutorialPopupCollection")
	TArray<TObjectPtr<UTutorialPopupContent>> TutorialPopupAssets;
};

USTRUCT(BlueprintType)
struct FTutorialPopupWidgetParams
{
	GENERATED_BODY()

	FTutorialPopupWidgetParams() = default;

	FTutorialPopupWidgetParams(bool bInPauseWhenAdded, bool bInUnpauseWhenRemoved);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bPauseWhenAdded = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUnpauseWhenRemoved = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCollectionFinishedSignature, UTutorialPopupCollection*, FinishedCollection);

/**
 * A popup tutorial widget meant to display game onboarding and information.
 * Features a Collection asset with multiple individual pages, that can be flipped through in both directions.
 */
UCLASS()
class VERTICALSLICE_API UTutorialPopupWidget : public UExtendedCommonActivatableWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "TutorialPopup")
	void AssignContentAsset(UTutorialPopupCollection* Asset, const FTutorialPopupWidgetParams& Params);

	// Displays the next content asset in the TutorialPopupCollection.
	// If we are at the last page, closes the widget.
	UFUNCTION(BlueprintCallable, Category = "TutorialPopup")
	void RequestNextPage();

	// Displays the previous content asset in the TutorialPopupCollection
	UFUNCTION(BlueprintCallable, Category = "TutorialPopup")
	void RequestPreviousPage();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Content Changed"), Category = "TutorialPopup")
	void K2_OnContentChanged();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Set Params"), Category = "TutorialPopup")
	void K2_OnSetParams(const FTutorialPopupWidgetParams& NewParams);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TutorialPopup")
	UTutorialPopupContent* GetCurrentContent();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TutorialPopup")
	bool IsLastPage() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TutorialPopup")
	bool IsFirstPage() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "TutorialPopup")
	int32 GetPageCount() const;

	UFUNCTION(BlueprintCallable, Category = "TutorialPopup")
	void CloseWidget();

protected:

	void SetDisplayedContentIndex(int32 NewIndex);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTutorialPopupCollection> ContentAsset = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FTutorialPopupWidgetParams Params;

	UPROPERTY(BlueprintReadOnly)
	int32 DisplayedContentIndex = 0;

public:
	// --- Delegates

	// Broadcasted when a TutorialPopupCollection has been finished.
	UPROPERTY(BlueprintAssignable, Category = "TutorialPopup")
	FOnCollectionFinishedSignature OnCollectionFinishedDelegate;
};