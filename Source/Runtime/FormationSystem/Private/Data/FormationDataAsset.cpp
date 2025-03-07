// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Data/FormationDataAsset.h"

#include "Components/FormationComponent.h"

void UFormationDataAsset::ExtractTransforms(const TArray<UFormationComponent*>& Units,
	TArray<FTransform>& OutTransforms)
{
	for (const UFormationComponent* Unit : Units)
	{
		if (IsValid(Unit))
		{
			OutTransforms.Add(Unit->GetOwner()->GetActorTransform());
		}
	}
}

FVector UFormationDataAsset::GetCenterOffset_Implementation()
{
	return FVector::ZeroVector;
}

void UFormationDataAsset::GetWorldTransforms_Implementation(const TArray<UFormationComponent*>& Units, const FVector& Location,
                                                            const FVector& Direction, TArray<FTransform>& OutTransforms)
{
	GetOffsetTransforms(Units, OutTransforms);
	for (FTransform& Transform : OutTransforms)
	{
		const FVector WorldLocation = Location + Direction.ToOrientationQuat().RotateVector(Transform.GetLocation());
		Transform.SetLocation(WorldLocation);
	}
}

void UFormationDataAsset::GetOffsetTransforms_Implementation(const TArray<UFormationComponent*>& Units,
                                                             TArray<FTransform>& OutTransforms)
{
	ExtractTransforms(Units, OutTransforms);
}