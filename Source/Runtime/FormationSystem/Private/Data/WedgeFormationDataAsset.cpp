// Copyright to Kat Code Labs, SRL. All Rights Reserved.

#include "Data/WedgeFormationDataAsset.h"

void UWedgeFormationDataAsset::GetOffsetTransforms_Implementation(const TArray<TScriptInterface<IFormationUnit>>& Units,
                                                                  TArray<FTransform>& OutTransforms)
{
	Super::GetOffsetTransforms_Implementation(Units, OutTransforms);
	
	int CurrentRow = 1;
	int FlipFlop = 1;
	const float HalfAngle = Angle / 2;

	OutTransforms[0].SetRotation(FQuat::Identity);
	OutTransforms[0].SetLocation(FVector::ZeroVector);
	
	for (int Index = 1; Index < OutTransforms.Num(); Index++)
	{
		FTransform& Transform = OutTransforms[Index];
		
		const FVector Location = FRotator(0.0f, FlipFlop * HalfAngle, 0.0f).RotateVector(-FVector::ForwardVector);
		Transform.SetLocation(Location * CurrentRow * Padding);
		Transform.SetRotation(FQuat::Identity);

		if (FlipFlop == -1)
		{
			CurrentRow++;
		}
		FlipFlop *= -1;
	}
}
