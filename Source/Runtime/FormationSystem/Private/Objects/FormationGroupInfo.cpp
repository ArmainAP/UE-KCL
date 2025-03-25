// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Objects/FormationGroupInfo.h"
#include "Data/FormationDataAsset.h"
#include "Interfaces/FormationUnit.h"
#include "Kismet/GameplayStatics.h"

bool UFormationGroupInfo::AddUnit(const TScriptInterface<IFormationUnit>& Unit)
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

bool UFormationGroupInfo::RemoveUnit(const TScriptInterface<IFormationUnit>& Unit)
{	
	if (!Unit.GetInterface())
	{
		return false;
	}

	UObject* Object = Unit.GetObject();
	if (!IsValid(Object))
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
	Unit->Execute_HandleFormationLeft(Object, this);
	return true;
}

void UFormationGroupInfo::StopMovement()
{
	for (const TScriptInterface<IFormationUnit>& Unit : Units)
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

	const int Count = Units.Num();
	TArray<FTransform> WorldTransforms;
	FormationDataAsset->GetWorldTransforms(Count, Location, Direction, WorldTransforms);

	for (int Index = 0; Index < Count; Index++)
	{
		if (const TScriptInterface<IFormationUnit>& Unit = Units[Index]; Unit.GetInterface())
		{
			Unit->Execute_SetupTarget(Unit.GetObject(), WorldTransforms[Index]);
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

TArray<TScriptInterface<IFormationUnit>> UFormationGroupInfo::GetUnits()
{
	return Units;
}

UFormationDataAsset* UFormationGroupInfo::GetFormationDataAsset() const
{
	return FormationDataAsset;
}

FVector UFormationGroupInfo::GetFormationAverageLocation() const
{
	TArray<AActor*> Actors;
	for (const TScriptInterface<IFormationUnit>& Unit : Units)
	{
		Actors.Add(Unit->Execute_GetActor(Unit.GetObject()));
	}
	return UGameplayStatics::GetActorArrayAverageLocation(Actors);
}

AActor* UFormationGroupInfo::GetFormationLead()
{
	if (!Units.Num())
	{
		return nullptr;
	}

	float LeadDistance = FLT_MAX;
	TScriptInterface<IFormationUnit> Lead = nullptr;
	for (const TScriptInterface<IFormationUnit>& Unit : Units)
	{
		const float Distance = Unit->Execute_GetDistanceToDestination(Unit.GetObject());
		if (Distance < LeadDistance)
		{
			LeadDistance = Distance;
			Lead = Unit;
		}
	}

	return Lead ? Lead->Execute_GetActor(Lead.GetObject()) : nullptr;
}

FVector UFormationGroupInfo::GetFormationLeadLocation()
{
	const AActor* Lead = GetFormationLead();
	return IsValid(Lead) ? Lead->GetActorLocation() : FVector::ZeroVector;
}
