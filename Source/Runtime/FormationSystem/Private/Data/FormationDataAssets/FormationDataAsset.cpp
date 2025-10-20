// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Data/FormationDataAssets/FormationDataAsset.h"

void UFormationDataAsset::GetOffsetTransforms_Implementation(const int UnitCount, TArray<FTransform>& OutTransforms) const
{
	const int32 CappedUnitCount = GetCappedUnitCount(UnitCount);
	if (CappedUnitCount < 1)
	{
		return;
	}
	
	OutTransforms.Reset();
	for (int Index = 0; Index < CappedUnitCount; Index++)
	{
		OutTransforms.Add(GetOffsetTransformForIndex(Index, UnitCount));
	}
}

FTransform UFormationDataAsset::GetOffsetTransformForIndex_Implementation(const int Index, const int UnitCount) const
{
	return FTransform::Identity;
}

int UFormationDataAsset::GetCappedUnitCount(const int UnitCount) const
{
	return (FormationLimit > INDEX_NONE) ? FMath::Min(UnitCount, FormationLimit) : UnitCount;
}
