// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScenarioTypes.h"
#include "EncounterTypes.h"
#include "Math/RandomStream.h"
#include "ScenarioVolume.generated.h"

class UScenario;
class AScenarioVolume;
class UBoxComponent;
class UScenarioUserComponent;
class UTextRenderComponent;

constexpr int32 NO_SEED = 0;

UCLASS()
class VERTICALSLICE_API AScenarioVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AScenarioVolume(const FObjectInitializer& ObjectInitializer);

	// --- Begin UObject Interface
	virtual void Serialize(FArchive& Ar) override;

	virtual void BeginDestroy() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	virtual void PostEditImport() override;

	virtual void PostDuplicate(bool bDuplicateForPIE) override;

	// --- End UObject Interface

	// --- Begin AActor Interface
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Destroyed() override;

#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished) override;

	virtual void EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;

	virtual void Tick(float DeltaTime) override;

	virtual bool ShouldTickIfViewportsOnly() const override;
	// --- End AActor Interface

	void OnLevelActorsModified(AActor* ModifiedActor);

	void OnLevelActorDeleted(AActor* Actor);

	void BindToEngine();

	void UnbindFromEngine();

	void FindComponentsInVolume();

	// Tries to immediately conduct the FindComponentsInVolume search if it's queued.
	void RequestImmediateSearch();

	bool bQueuedSearch = false;

	// Randomizes the seed used for any random properties.
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "ScenarioVolume")
	void RandomizeSeed();

	// Effectively removes any active randomization.
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "ScenarioVolume")
	void ResetSeed();

#endif // WITH_EDITOR

	// Shuffles any properties that randomize by the Seed value
	void ShuffleBySeed();

	void BindToAssets();

	void UnbindFromAssets();

	// An Asset we depend on has been edited, moved or removed.
	void OnAssetInvalidated();

	UScenario* GetActiveScenario() const;

	UEncounter* GetActiveEncounter() const;

	// Should only be used at runtime.
	void InjectScenario(UScenario* InScenario);
 
	[[nodiscard]] FEncounterEntry GetEnemyEntryById(ScenarioUserId Id);

	[[nodiscard]] ScenarioUserId GetNextParticipantId();

	void ReturnId(ScenarioUserId Id);

	// Tries to iteratively decrement the Id, to avoid storing redundant Ids.
	void TryDecrementId();

	// Clears all tracked components and treats them as having Exited this volume.
	void ClearComponents();

	// Shuffles it when received.
	void FetchLocalDistribution();

	// Makes the two components swap Id with eachother.
	void SwapIds(ScenarioUserId Left, ScenarioUserId Right);

	void SwapIds(UScenarioUserComponent* Left, UScenarioUserComponent* Right);

	void GetUsers(TArray<UScenarioUserComponent*>& OutUsers) const;

protected:

	void SetSeed(int32 Seed);

	void UpdateTextRenderer();

	UPROPERTY(EditAnywhere, Category = "ScenarioVolume")
	TObjectPtr<UScenario> Scenario = nullptr;

	// Components covered by the Volume
	UPROPERTY(VisibleAnywhere)
	TSet<TObjectPtr<UScenarioUserComponent>> ContainedComponents;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> Volume = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTextRenderComponent> TextRenderer = nullptr;
#endif //WITH_EDITORONLY_DATA

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> CustomRoot = nullptr;

	ScenarioUserId NextParticipantId = 1U;

	TArray<ScenarioUserId> VacantIds;

	TArray<int32> LocalEnemyDistribution;

	int32 Seed = NO_SEED;

public:

	// --- Delegates
	FOnAssetChainInvalidatedSignature OnAssetChainInvalidatedDelegate;
};
