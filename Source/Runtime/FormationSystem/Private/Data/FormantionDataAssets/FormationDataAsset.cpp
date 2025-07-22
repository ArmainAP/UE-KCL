// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Data/FormationDataAssets/FormationDataAsset.h"

void UFormationDataAsset::GetWorldTransforms_Implementation(const int UnitCount, const FVector& Location,
                                                            const FVector& Direction, TArray<FTransform>& OutTransforms) const
{
	GetOffsetTransforms(UnitCount, OutTransforms);
	for (FTransform& Transform : OutTransforms)
	{
		const FVector WorldLocation = Location + Direction.ToOrientationQuat().RotateVector(Transform.GetLocation());
		Transform.SetLocation(WorldLocation);
		Transform.SetRotation(Direction.ToOrientationQuat());
	}
}

void UFormationDataAsset::GetOffsetTransforms_Implementation(const int UnitCount, TArray<FTransform>& OutTransforms) const
{
	OutTransforms.Reset();
	OutTransforms.AddDefaulted(UnitCount);
}