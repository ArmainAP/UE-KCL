// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Data/FormationDataAsset.h"

#include "Components/FormationComponent.h"

void UFormationDataAsset::ExtractTransforms(const TArray<TScriptInterface<IFormationUnit>>& Units,
                                            TArray<FTransform>& OutTransforms)
{
	for (TScriptInterface<IFormationUnit> Unit : Units)
	{
		if (Unit.GetInterface())
		{
			OutTransforms.Add(Unit->Execute_GetTransform(Unit.GetObject()));
		}
	}
}

void UFormationDataAsset::GetWorldTransforms_Implementation(const TArray<TScriptInterface<IFormationUnit>>& Units,
	const FVector& Location, const FVector& Direction, TArray<FTransform>& OutTransforms)
{
	GetOffsetTransforms(Units, OutTransforms);
	for (FTransform& Transform : OutTransforms)
	{
		const FVector WorldLocation = Location + Direction.ToOrientationQuat().RotateVector(Transform.GetLocation());
		Transform.SetLocation(WorldLocation);
	}
}

void UFormationDataAsset::GetOffsetTransforms_Implementation(const TArray<TScriptInterface<IFormationUnit>>& Units,
                                                             TArray<FTransform>& OutTransforms)
{
	ExtractTransforms(Units, OutTransforms);
}