// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/FormationDataAssets/RowFormationDataAsset.h"

int URowFormationDataAsset::GetTotalRows(const int ObjectCount) const
{
	return FMath::RoundToInt(static_cast<float>(ObjectCount / MaxColumns)); 
}

void URowFormationDataAsset::GetOffsetTransforms_Implementation(const int UnitCount, TArray<FTransform>& OutTransforms)
{
	Super::GetOffsetTransforms_Implementation(UnitCount, OutTransforms);

	if (UnitCount == 0) return;

	const int ColumnsCount = FMath::Min(MaxColumns, UnitCount);
	const int TotalRows = FMath::CeilToInt(static_cast<float>(UnitCount) / ColumnsCount);

	int Index = 0;
	for (int RowIndex = 0; RowIndex < TotalRows; RowIndex++)
	{
		for (int ColumnIndex = ColumnsCount - 1; ColumnIndex >= 0; ColumnIndex--) // Reverse column order
		{
			if (Index >= UnitCount) break;

			FTransform& Transform = OutTransforms[Index];

			const float ColumnOffset = (ColumnIndex - (ColumnsCount - 1) * 0.5f) * Padding.Y;
			const float RowOffset = -RowIndex * Padding.X; 

			const FVector Position = FVector(RowOffset, ColumnOffset, 0.0f);
			Transform.SetLocation(Position);
			Transform.SetRotation(FQuat::Identity);

			Index++;
		}
	}
}