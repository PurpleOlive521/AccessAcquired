// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Level/ScenarioVolume.h"
#include "Scenario.h"
#include "ScenarioUserComponent.h"
#include "ScenarioCustomVersion.h"
#include "Encounter.h"
#include "DevCommons.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"

AScenarioVolume::AScenarioVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

#if WITH_EDITORONLY_DATA
	PrimaryActorTick.bCanEverTick = true;
	bRunConstructionScriptOnDrag = false;
#endif //WITH_EDITORONLY_DATA

	CustomRoot = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("CustomSceneRoot"));
	CustomRoot->SetMobility(EComponentMobility::Static);
	SetRootComponent(CustomRoot);

	Volume = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Volume"));
	Volume->SetMobility(EComponentMobility::Static);
	Volume->SetupAttachment(CustomRoot);

#if WITH_EDITORONLY_DATA
	TextRenderer = ObjectInitializer.CreateEditorOnlyDefaultSubobject<UTextRenderComponent>(this, TEXT("Scenario Name"));
	if (TextRenderer)
	{
		TextRenderer->SetupAttachment(CustomRoot);
		TextRenderer->SetMobility(EComponentMobility::Movable);
		TextRenderer->WorldSize = 200.0f;
		TextRenderer->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
		TextRenderer->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);

		const FQuat Rotation = FQuat::MakeFromEuler(FVector(0.0f, 90.0f, 0.0f));
		TextRenderer->SetWorldRotation(Rotation);

		const FVector Offset = { 0.0f, 0.0f, 650.0f };
		TextRenderer->SetRelativeLocation(Offset);
	}
#endif // WITH_EDITORONLY_DATA

}

void AScenarioVolume::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	Ar.UsingCustomVersion(FScenarioCustomVersion::GUID);

	if (Ar.IsLoading() && Ar.CustomVer(FScenarioCustomVersion::GUID) < FScenarioCustomVersion::InitialCustomProperties)
	{
		// No custom serialization added yet, don't try to load non-existent data
		return;
	}

	Ar << NextParticipantId;
	Ar << VacantIds;
	Ar << Seed;	
}

// UObject response to being GC'd, last chance to unbind if it failed elsewhere
void AScenarioVolume::BeginDestroy()
{
#if WITH_EDITOR
	UnbindFromEngine();
#endif //WITH_EDITOR

	UnbindFromAssets();

	Super::BeginDestroy();
}

#if WITH_EDITOR
void AScenarioVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	OnAssetInvalidated();
}
#endif //WITH_EDITOR

void AScenarioVolume::PostEditImport()
{
	Super::PostEditImport();

	ContainedComponents.Empty();
}

void AScenarioVolume::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);

	if (!bDuplicateForPIE)
	{
		ContainedComponents.Empty();
	}
}

void AScenarioVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// No bindings for temporaries
	if (IsTemplate(RF_Transient))
	{
		return;
	}

#if WITH_EDITOR
	UnbindFromEngine();
	BindToEngine();
	bQueuedSearch = true;
#endif //WITH_EDITOR

	UnbindFromAssets();
	BindToAssets();

	FetchLocalDistribution();
	UpdateTextRenderer();
}

void AScenarioVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		ClearComponents();
	}

	Super::EndPlay(EndPlayReason);
}

void AScenarioVolume::Destroyed()
{
#if WITH_EDITOR
	UnbindFromEngine();
#endif //WITH_EDITOR

	UnbindFromAssets();

	Super::Destroyed();
}

#if WITH_EDITOR
void AScenarioVolume::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	if (bFinished)
	{
		bQueuedSearch = true;
	}
}

// Shares implementation with TriggerBox.h and similar volumes for easier scaling in the Editor.
void AScenarioVolume::EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	const FVector ModifiedScale = DeltaScale * (AActor::bUsePercentageBasedScaling ? 500.0f : 5.0f);

	if (bCtrlDown)
	{
		// CTRL+Scaling modifies trigger collision height.  This is for convenience, so that height
		// can be changed without having to use the non-uniform scaling widget (which is
		// inaccessable with spacebar widget cycling).
		FVector Extent = Volume->GetUnscaledBoxExtent() + FVector(0, 0, ModifiedScale.X);
		Extent.Z = FMath::Max<FVector::FReal>(0, Extent.Z);
		Volume->SetBoxExtent(Extent);
	}
	else
	{
		FVector Extent = Volume->GetUnscaledBoxExtent() + FVector(ModifiedScale.X, ModifiedScale.Y, ModifiedScale.Z);
		Extent.X = FMath::Max<FVector::FReal>(0, Extent.X);
		Extent.Y = FMath::Max<FVector::FReal>(0, Extent.Y);
		Extent.Z = FMath::Max<FVector::FReal>(0, Extent.Z);
		Volume->SetBoxExtent(Extent);
	}
}

void AScenarioVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bQueuedSearch)
	{
		FindComponentsInVolume();
	}
}

bool AScenarioVolume::ShouldTickIfViewportsOnly() const
{
	return true;
}

void AScenarioVolume::OnLevelActorsModified(AActor* ModifiedActor)
{
	if (ModifiedActor == this)
	{
		return;
	}

	bQueuedSearch = true;
}

void AScenarioVolume::OnLevelActorDeleted(AActor* Actor)
{
	if (Actor == this)
	{
		ClearComponents();
	}
}

void AScenarioVolume::BindToEngine()
{
	if (GEngine)
	{
		GEngine->OnActorMoved().AddUObject(this, &AScenarioVolume::OnLevelActorsModified);
		GEngine->OnLevelActorDeleted().AddUObject(this, &AScenarioVolume::OnLevelActorDeleted);
	}
}

void AScenarioVolume::UnbindFromEngine()
{
	if (GEngine)
	{
		GEngine->OnActorMoved().RemoveAll(this);
	}
}

void AScenarioVolume::FindComponentsInVolume()
{
	// Guard against recursive calls by setting the flag first
	bQueuedSearch = false;

	TSet<TObjectPtr<UScenarioUserComponent>> ComponentsCopy = ContainedComponents;

	TArray<FOverlapResult> OutResults;

	const FVector Position = GetActorLocation();
	const FRotator Rotation = GetActorRotation();
	const ECollisionChannel Channel = ECollisionChannel::ECC_WorldStatic;
	FComponentQueryParams ComponentParams = { FName("AScenarioVolume:FindActors"), this };
	FCollisionObjectQueryParams ObjectParams = {};

	Volume->ComponentOverlapMulti(OutResults, GetWorld(), Position, Rotation, Channel, ComponentParams);
	for (const FOverlapResult& Result : OutResults)
	{
		AActor* Actor = Result.GetActor();

		if (Actor->GetLevel() != GetLevel())
		{
			continue;
		}

		TArray<UScenarioUserComponent*> OutComps;
		Actor->GetComponents(UScenarioUserComponent::StaticClass(), OutComps, true);

		for (UScenarioUserComponent* Component : OutComps)
		{
			if (!ContainedComponents.Contains(Component))
			{
				ContainedComponents.Add(Component);
				Component->OnEnteredScenario(this);
			}

			ComponentsCopy.Remove(Component);
		}
	}

	for (auto Component : ComponentsCopy)
	{
		ContainedComponents.Remove(Component);

		if (Component)
		{
			Component->OnExitedScenario(this);
		}
	}
}

void AScenarioVolume::RequestImmediateSearch()
{
	if (bQueuedSearch)
	{
		FindComponentsInVolume();
	}
}

void AScenarioVolume::RandomizeSeed()
{
	SetSeed(FMath::Rand32());
	OnAssetInvalidated();
}

void AScenarioVolume::ResetSeed()
{
	SetSeed(NO_SEED);
	OnAssetInvalidated();
}

#endif //WITH_EDITOR

void AScenarioVolume::ShuffleBySeed()
{
	if (Seed == NO_SEED)
	{
		return;
	}

	if (LocalEnemyDistribution.Num() != 0)
	{
		FRandomStream Random = { Seed };

		int32 LastIndex = LocalEnemyDistribution.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = Random.RandRange(i, LastIndex);
			if (i != Index)
			{
				LocalEnemyDistribution.Swap(i, Index);
			}
		}
	}
}

void AScenarioVolume::BindToAssets()
{
	if (Scenario)
	{
		Scenario->OnAssetChainInvalidatedDelegate.AddUObject(this, &AScenarioVolume::OnAssetInvalidated);
	}
}

void AScenarioVolume::UnbindFromAssets()
{
	if (Scenario)
	{
		Scenario->OnAssetChainInvalidatedDelegate.RemoveAll(this);
	}
}

void AScenarioVolume::OnAssetInvalidated()
{
	FetchLocalDistribution();

	UpdateTextRenderer();

	OnAssetChainInvalidatedDelegate.Broadcast();
}

UScenario* AScenarioVolume::GetActiveScenario() const
{
	return Scenario;
}

UEncounter* AScenarioVolume::GetActiveEncounter() const
{
	if (!Scenario)
	{
		return nullptr;
	}

	return Scenario->GetCurrentEncounter();
}

void AScenarioVolume::InjectScenario(UScenario* InScenario)
{
	Scenario = InScenario;

	OnAssetInvalidated();
}

FEncounterEntry AScenarioVolume::GetEnemyEntryById(ScenarioUserId Id)
{
	if (LocalEnemyDistribution.IsEmpty())
	{
		// Retry fetch in case this is triggered by a race condition on BeginPlay
		FetchLocalDistribution();

		if (LocalEnemyDistribution.IsEmpty())
		{
			return FEncounterEntry();
		}
	}

	const int EntryIndex = LocalEnemyDistribution[Id % LocalEnemyDistribution.Num()];

	UEncounter* Encounter = GetActiveEncounter();
	if (!Encounter)
	{
		return FEncounterEntry();
	}

	return Encounter->GetEntryAtIndex(EntryIndex);
}

ScenarioUserId AScenarioVolume::GetNextParticipantId()
{
	ScenarioUserId NewId = INVALID_SCENARIO_USER_ID;

	if (VacantIds.Num() >= 1)
	{
		NewId = VacantIds.Pop(EAllowShrinking::No);
		TryDecrementId();
		return NewId;
	}

	NewId = NextParticipantId++;
	return NewId;
}

void AScenarioVolume::ReturnId(ScenarioUserId Id)
{
	if (Id == INVALID_SCENARIO_USER_ID)
	{
		return;
	}

	VacantIds.Add(Id);
	TryDecrementId();
}

void AScenarioVolume::TryDecrementId()
{
	if (VacantIds.IsEmpty())
	{
		VacantIds.Shrink();
		return;
	}

	if (VacantIds.Last() == NextParticipantId - 1)
	{
		NextParticipantId = VacantIds.Pop(EAllowShrinking::No);
		TryDecrementId();
	}
}

void AScenarioVolume::ClearComponents()
{
	for (auto Component : ContainedComponents)
	{
		if (Component)
		{
			Component->OnExitedScenario(this);
		}
	}

	ContainedComponents.Empty();
}

void AScenarioVolume::FetchLocalDistribution()
{
	// Don't hold on to stale state
	LocalEnemyDistribution.Empty();

	if (UEncounter* Encounter = GetActiveEncounter())
	{
		Encounter->GetCurrentDistribution(LocalEnemyDistribution);

		ShuffleBySeed();
		
		return;
	}
}

void AScenarioVolume::SwapIds(ScenarioUserId LeftId, ScenarioUserId RightId)
{
	TArray<UScenarioUserComponent*> Comps = ContainedComponents.Array();

	UScenarioUserComponent* LeftComp = nullptr;
	UScenarioUserComponent* RightComp = nullptr;
	for (const auto& Element : Comps)
	{
		if (Element->GetUserId() == LeftId)
		{
			LeftComp = Element;
		}

		if (Element->GetUserId() == RightId)
		{
			RightComp = Element;
		}
	}
	
	if (LeftComp && RightComp)
	{
		ScenarioUserId Temp = LeftComp->GetUserId();
		LeftComp->SetUserId(RightComp->GetUserId());
		RightComp->SetUserId(Temp);
	}
}

void AScenarioVolume::SwapIds(UScenarioUserComponent* Left, UScenarioUserComponent* Right)
{
	if (ContainedComponents.Contains(Left) && ContainedComponents.Contains(Right))
	{
		SwapIds(Left->GetUserId(), Right->GetUserId());
	}
}

void AScenarioVolume::GetUsers(TArray<UScenarioUserComponent*>& OutUsers) const
{
	OutUsers = ContainedComponents.Array();
}

void AScenarioVolume::SetSeed(int32 InSeed)
{
	if (Seed == InSeed)
	{
		return;
	}

	Seed = InSeed;
}

void AScenarioVolume::UpdateTextRenderer()
{
#if WITH_EDITORONLY_DATA
	if (Scenario)
	{
		const FText ScenarioName = FText::FromString(Scenario->GetName());
		TextRenderer->SetText(ScenarioName);
	}
	else
	{
		const FText EmptyName = INVTEXT("No Scenario");
		TextRenderer->SetText(EmptyName);
	}
#endif //WITH_EDITORONLY_DATA
}
