// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/FormationDataAssets/RowFormationDataAsset.h"

FTransform URowFormationDataAsset::GetOffsetTransformForIndex_Implementation(const int Index, const int UnitCount) const
{
	if (!Index)
	{
		return FTransform::Identity;
	}
	
	const uint32 ColumnsCount = FMath::Min(MaxColumns, UnitCount);
	
	const float HalfSpan = (ColumnsCount - 1) * 0.5f;
		
	const int32 RowIndex = Index / ColumnsCount;
	const int32 InRowIndex = Index % ColumnsCount;
	const int32 ColumnIndex = (ColumnsCount - 1) - InRowIndex;

	const float ColumnOffset = (ColumnIndex - HalfSpan) * Padding.Y;
	const float RowOffset = -RowIndex * Padding.X;

	FTransform Transform;
	Transform.SetLocation(FVector(RowOffset, ColumnOffset, 0.0f));
	Transform.SetRotation(FQuat::Identity);
	return Transform;
}
