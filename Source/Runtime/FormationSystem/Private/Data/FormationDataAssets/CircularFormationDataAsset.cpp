// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Data/FormationDataAssets/CircularFormationDataAsset.h"

FTransform UCircularFormationDataAsset::GetOffsetTransformForIndex_Implementation(const int Index,
	const int UnitCount) const
{
	// Compute step: include both endpoints for arcs, exclude duplicate for full circle
	const int32 CappedUnitCount = GetCappedUnitCount(UnitCount);
	const bool bFullCircle = FMath::IsNearlyEqual(FMath::Abs(FMath::Fmod(Degrees, 360.f)), 360.f, 0.5f);
	Segments = bFullCircle ? (Degrees / CappedUnitCount) : ((CappedUnitCount > 1) ? (Degrees / (CappedUnitCount - 1)) : 0.f);

	const float Angle = StartAngle + Index * Segments;
	const FVector Local = FRotator(0.f, Angle, 0.f).RotateVector(FVector(Radius, 0.f, 0.f));
	return FTransform(FQuat::Identity, Local);
}
