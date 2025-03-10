// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/SquareFormationDataAsset.h"

void USquareFormationDataAsset::GetOffsetTransforms_Implementation(const TArray<TScriptInterface<IFormationUnit>>& Units, 
	TArray<FTransform>& OutTransforms)
{
	Super::GetOffsetTransforms_Implementation(Units, OutTransforms);

	int CachedTransformCount = OutTransforms.Num();
	if (CachedTransformCount == 0) return;  // Early exit for empty input

	// Calculate the number of units per edge
	constexpr int TotalEdges = 4;
	const int UnitsPerEdge = FMath::CeilToInt(static_cast<float>(CachedTransformCount) / TotalEdges);
	const float EdgeLength = Padding * UnitsPerEdge; // Total square edge length

	int Index = 0;
	for (int Edge = 0; Edge < TotalEdges; Edge++)
	{
		for (int UnitIndex = 0; UnitIndex < UnitsPerEdge; UnitIndex++)
		{
			if (Index >= CachedTransformCount) return;  // Stop when all units are placed

			FTransform& Transform = OutTransforms[Index];

			// Calculate the unit's position along the current edge
			float UnitOffset = (UnitIndex * Padding) - (EdgeLength * 0.5f); // Centered position

			FVector Position;
			switch (Edge)
			{
			case 0: Position = FVector(EdgeLength * 0.5f, UnitOffset, 0.0f); break;  // Top Edge
			case 1: Position = FVector(-UnitOffset, EdgeLength * 0.5f, 0.0f); break; // Right Edge
			case 2: Position = FVector(-EdgeLength * 0.5f, -UnitOffset, 0.0f); break; // Bottom Edge
			case 3: Position = FVector(UnitOffset, -EdgeLength * 0.5f, 0.0f); break; // Left Edge
			}

			Transform.SetLocation(Position);
			Transform.SetRotation(FQuat::Identity); // Default rotation

			Index++;
		}
	}
}