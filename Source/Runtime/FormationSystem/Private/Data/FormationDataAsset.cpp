// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Data/FormationDataAsset.h"

void UFormationDataAsset::ExtractTransforms(const TArray<AActor*>& Actors, TArray<FTransform>& OutTransforms)
{
	for (const AActor* Actor : Actors)
	{
		if (IsValid(Actor))
		{
			OutTransforms.Add(Actor->GetActorTransform());
		}
	}
}

FVector UFormationDataAsset::GetCenterOffset_Implementation()
{
	return FVector::ZeroVector;
}

void UFormationDataAsset::GetWorldTransforms_Implementation(const TArray<AActor*>& Actors, const FVector& Location,
                                                            const FVector& Direction, TArray<FTransform>& OutTransforms)
{
	GetOffsetTransforms(Actors, OutTransforms);
	for (FTransform& Transform : OutTransforms)
	{
		const FVector WorldLocation = Location + Direction.ToOrientationQuat().RotateVector(Transform.GetLocation());
		Transform.SetLocation(WorldLocation);
	}
}

void UFormationDataAsset::GetOffsetTransforms_Implementation(const TArray<AActor*>& Actors,
                                                             TArray<FTransform>& OutTransforms)
{
	ExtractTransforms(Actors, OutTransforms);
}