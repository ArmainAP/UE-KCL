// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Data/CircularFormationDataAsset.h"
#include "Kismet/KismetMathLibrary.h"

void UCircularFormationDataAsset::GetOffsetTransforms_Implementation(const TArray<UFormationComponent*>& Units,
	TArray<FTransform>& OutTransforms)
{
	Super::GetOffsetTransforms_Implementation(Units, OutTransforms);

	const int Count = OutTransforms.Num();
	const int Segments = Degrees / Count;
	
	for (int Index = 0; Index < Count; Index++)
	{
		FTransform& Transform = OutTransforms[Index];
		const FRotator CircleRotation = FRotator(0.0f, Index * Segments, 0.0f);
		const FVector CircleLocation = UKismetMathLibrary::GetForwardVector(CircleRotation) * Radius;
		Transform.SetLocation(CircleLocation);
		Transform.SetRotation(CircleRotation.Quaternion());
	}
}
