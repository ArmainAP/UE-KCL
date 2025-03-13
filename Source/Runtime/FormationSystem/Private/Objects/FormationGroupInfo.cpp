// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Objects/FormationGroupInfo.h"
#include "Data/FormationDataAsset.h"
#include "Interfaces/FormationUnit.h"

bool UFormationGroupInfo::AddUnit(TScriptInterface<IFormationUnit> Unit)
{
	if (!Unit.GetInterface())
	{
		return false;
	}

	if (Units.Contains(Unit))
	{
		return false;
	}

	Units.AddUnique(Unit);
	OnFormationUnitJoined.Broadcast(Unit);
	Unit->Execute_HandleFormationJoined(Unit.GetObject(), this);
	return true;
}

bool UFormationGroupInfo::RemoveUnit(TScriptInterface<IFormationUnit> Unit)
{
	if (!Unit.GetInterface())
	{
		return false;
	}

	const int Index = Units.Find(Unit);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	Units.RemoveAt(Index);
	OnFormationUnitLeft.Broadcast(Unit);
	Unit->Execute_HandleFormationLeft(Unit.GetObject(), this);
	return true;
}

void UFormationGroupInfo::StopMovement()
{
	for (const TScriptInterface<IFormationUnit> Unit : Units)
	{
		if (Unit.GetInterface())
		{
			Unit->Execute_StopMovement(Unit.GetObject());
		}
	}
}

void UFormationGroupInfo::MoveFormation(const FVector& Location, const FVector& Direction)
{
	if (!IsValid(FormationDataAsset))
	{
		return;
	}

	TArray<FTransform> WorldTransforms;
	FormationDataAsset->GetWorldTransforms(Units, Location, Direction, WorldTransforms);

	for (int Index = 0; Index < Units.Num(); Index++)
	{
		if (const TScriptInterface<IFormationUnit> Unit = Units[Index]; Unit.GetInterface())
		{
			const FTransform& Transform = WorldTransforms[Index];
			const FVector WorldLocation = Transform.GetLocation();
			const FRotator Rotation = FRotator(Transform.GetRotation() * Direction.ToOrientationQuat());
			Unit->Execute_SetupTarget(Unit.GetObject(), WorldLocation, Rotation);
		}
	}
}

void UFormationGroupInfo::SetFormationDataAsset(UFormationDataAsset* InFormationDataAsset)
{
	FormationDataAsset = InFormationDataAsset;
}

int UFormationGroupInfo::GetUnitsCount() const
{
	return Units.Num();
}

UFormationDataAsset* UFormationGroupInfo::GetFormationDataAsset() const
{
	return FormationDataAsset;
}
