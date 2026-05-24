// Copyright (c) 2026, Oliver Österlund Stare. All rights reserved.


#include "Encounter.h"
#include "DevCommons.h"
#include "Misc/DataValidation.h"

FDistributionPrediction::FDistributionPrediction(uint32 Overrides, uint32 Size)
{
	TotalOverrides = Overrides;
	FractionSize = Size;
}

inline uint32 FDistributionPrediction::GetTotalCount() const
{
	return TotalOverrides + FractionSize;
}

#if WITH_EDITOR
EDataValidationResult UEncounter::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	bool bOnlyOverrides = true;
	float SumRatios = 0.0f;
	for (const FEncounterEntry& Entry : Entries)
	{
		if (Entry.OverrideCount == NO_OVERRIDE)
		{
			if (!Entry.Fraction.IsProperInclusive())
			{
				Context.AddError(INVTEXT("Entry has improper fraction. Only proper fractions are supported!"));
				Result = EDataValidationResult::Invalid;
			}

			if (Entry.Fraction.HasZero())
			{
				Context.AddError(INVTEXT("Entry has zero value. Numerator and denominator must be above 0!"));
				Result = EDataValidationResult::Invalid;
			}

			bOnlyOverrides = false;
			SumRatios += Entry.Fraction.ToCoefficient();
		}
	}

	if (!bOnlyOverrides)
	{
		if (SumRatios > 1.0f)
		{
			Context.AddError(INVTEXT("Fractions total to above 100%. All fractions combined must be exactly 100%"));
			Result = EDataValidationResult::Invalid;
		}

		if (SumRatios < 1.0f)
		{
			Context.AddError(INVTEXT("Fractions total to under 100%. All fractions combined must be exactly 100%"));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}

void UEncounter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	GenerateDistributions();

	OnAssetInvalidated();
}

#endif //WITH_EDITOR

FPrimaryAssetId UEncounter::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("EncounterAssetItems", GetFName());
}

void UEncounter::GetCurrentDistribution(TArray<int>& OutArray)
{
	if (!bHasGeneratedDistribution)
	{
		GenerateDistributions();
	}

	OutArray = EntryDistribution;
}

FEncounterEntry UEncounter::GetEntryAtIndex(uint32 Index) const
{
	int64 SignedIndex = Index;
	if (SignedIndex >= Entries.Num())
	{
		return FEncounterEntry();
	}

	return Entries[SignedIndex];
}

void UEncounter::OnAssetInvalidated()
{
	OnAssetChainInvalidatedDelegate.Broadcast();
}

void UEncounter::GenerateDistributions()
{
	const FDistributionPrediction Distribution = GetRequiredDistributions();
	MinimumEnemyCount = 0;

	ensure(Distribution.GetTotalCount() >= 0);

	if (Distribution.GetTotalCount() == 0)
	{
		EntryDistribution.Empty();
		return;
	}

	if (Distribution.GetTotalCount() >= MAX_ALLOWED_DISTRIBUTION)
	{
		EntryDistribution.Empty();
		return;
	}

	EntryDistribution.SetNumZeroed(Distribution.GetTotalCount());

	int DistributionIndex = 0;
	int EntryIndex = 0;
	for (const auto& Entry : Entries)
	{
		int ToAdd = Entry.OverrideCount;

		if (ToAdd == NO_OVERRIDE)
		{
			if (Entry.Fraction.HasZero())
			{
				ToAdd = 0;
			}

			ToAdd = FMath::RoundToNearestTiesToEven(Distribution.FractionSize * Entry.Fraction.ToCoefficient());
		}

		for (int i = DistributionIndex; i < DistributionIndex + ToAdd; i++)
		{
			EntryDistribution[i] = EntryIndex;
		}

		EntryIndex++;
		DistributionIndex++;
	}

	MinimumEnemyCount = EntryDistribution.Num();
	bHasGeneratedDistribution = true;
}

FDistributionPrediction UEncounter::GetRequiredDistributions() const
{
	FDistributionPrediction Prediction; 
	
	bool bHasValidEntry = false;
	float TotalFraction = 0.0f;
	int CommonDenominator = 1;
	for (const auto& Entry : Entries)
	{
		if (Entry.OverrideCount != NO_OVERRIDE)
		{
			Prediction.TotalOverrides += Entry.OverrideCount;
			bHasValidEntry = true;
			continue;
		}

		if (Entry.Fraction.HasZero())
		{
			UE_LOG(VSLog, Error, TEXT("Encounter: Fraction with zero encountered. Can't generate accurate distribution."));
			return FDistributionPrediction();
		}

		if (!Entry.Fraction.IsProperInclusive())
		{
			UE_LOG(VSLog, Error, TEXT("Encounter: Improper fraction encountered. Can't generate accurate distribution."));
			return FDistributionPrediction();
		}
		
		if (TotalFraction + Entry.Fraction.ToCoefficient() > 1.0f)
		{
			UE_LOG(VSLog, Error, TEXT("Encounter: Fractions combine to over 100 percent. Can't generate accurate distribution."));
			return FDistributionPrediction();
		}

		TotalFraction += Entry.Fraction.ToCoefficient();

		CommonDenominator = FMath::LeastCommonMultiplier(CommonDenominator, Entry.Fraction.Denominator);
		bHasValidEntry = true;
	}

	if (!bHasValidEntry)
	{
		return FDistributionPrediction();
	}

	TArray<int> Sums;
	int GCD = 0;
	for (const auto& Entry : Entries)
	{
		const int Scaled = Entry.Fraction.Numerator * (CommonDenominator / Entry.Fraction.Denominator);
		Sums.Push(Scaled);

		if (GCD == 0)
		{
			GCD = Scaled;
			continue;
		}

		GCD = FMath::GreatestCommonDivisor(GCD, Scaled);
	}

	int RequiredDistributions = 0;
	for (int Sum : Sums)
	{
		RequiredDistributions += FMath::DivideAndRoundNearest(Sum, GCD);
	}

	Prediction.FractionSize = RequiredDistributions;
	return Prediction;
}
