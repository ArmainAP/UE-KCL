// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/RowFormationDataAsset.h"

int URowFormationDataAsset::GetTotalRows(const int ObjectCount) const
{
	return FMath::RoundToInt(static_cast<float>(ObjectCount / MaxColumns)); 
}

void URowFormationDataAsset::GetOffsetTransforms_Implementation(const TArray<UFormationComponent*>& Units,
	TArray<FTransform>& OutTransforms)
{
	Super::GetOffsetTransforms_Implementation(Units, OutTransforms);

	CachedTransformCount = OutTransforms.Num();
	if (CachedTransformCount == 0) return;

	const int ColumnsCount = FMath::Min(MaxColumns, CachedTransformCount);
	const int TotalRows = FMath::CeilToInt(static_cast<float>(CachedTransformCount) / ColumnsCount);

	int Index = 0;
	for (int RowIndex = 0; RowIndex < TotalRows; RowIndex++)
	{
		for (int ColumnIndex = 0; ColumnIndex < ColumnsCount; ColumnIndex++)
		{
			if (Index >= CachedTransformCount) break;

			FTransform& Transform = OutTransforms[Index];

			// Calculate centered column positions
			const float ColumnOffset = (ColumnIndex - (ColumnsCount - 1) * 0.5f) * Padding.Y;
			const float RowOffset = RowIndex * Padding.X;

			const FVector Position = FVector(RowOffset, ColumnOffset, 0.0f);
			Transform.SetLocation(Position);
			Transform.SetRotation(FQuat::Identity);
			
			Index++;
		}
	}
}

FVector URowFormationDataAsset::GetCenterOffset_Implementation()
{
	const float HalfRowPadding = (GetTotalRows(CachedTransformCount) - 1) * Padding.X / 2;
	return FVector(HalfRowPadding, 0.0f, 0.0f);
}
