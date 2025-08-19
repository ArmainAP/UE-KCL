// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/FormationDataAssets/WedgeFormationDataAsset.h"

FTransform UWedgeFormationDataAsset::GetOffsetTransformForIndex_Implementation(const int Index, const int UnitCount) const
{
	const float HalfAngle = Angle * 0.5f;
	const int32 CurrentRow = (Index + 1) / 2;
	const int32 FlipFlop = (Index & 1) ? 1 : -1;

	const FVector Dir = FRotator(0.f, FlipFlop * HalfAngle, 0.f).RotateVector(-FVector::ForwardVector);
	
	FTransform Transform;
	Transform.SetLocation(Dir * CurrentRow * Padding);
	Transform.SetRotation(FQuat::Identity);
	return Transform;
}