// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/FormationDataAssets/SquareFormationDataAsset.h"

FTransform USquareFormationDataAsset::GetOffsetTransformForIndex_Implementation(const int Index,
	const int UnitCount) const
{
	const int32 UnitsPerEdge = FMath::CeilToInt(static_cast<float>(UnitCount) / 4);
	const float EdgeLength = Padding * UnitsPerEdge;
	
	const int32 Edge = Index / UnitsPerEdge;
	const int32 UnitIndex = Index % UnitsPerEdge;

	const float UnitOffset = (UnitIndex * Padding) - (EdgeLength * 0.5f);

	FVector Position;
	switch (Edge)
	{
	case 0: Position = FVector(EdgeLength * 0.5f, UnitOffset, 0.0f); break;
	case 1: Position = FVector(-UnitOffset, EdgeLength * 0.5f, 0.0f); break;
	case 2: Position = FVector(-EdgeLength * 0.5f, -UnitOffset, 0.0f); break;
	default:/*3*/ Position = FVector( UnitOffset, -EdgeLength * 0.5f, 0.0f); break;
	}

	FTransform Transform;
	Transform.SetLocation(Position);
	Transform.SetRotation(FQuat::Identity);
	return Transform;
}
