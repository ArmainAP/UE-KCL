// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Subsystems/FormationSubsystem.h"

#include "Components/FormationComponent.h"
#include "Kismet/GameplayStatics.h"

bool UFormationSubsystem::CreateGroup(const FName GroupID, UFormationDataAsset* DataAsset)
{
	if (GroupID.IsNone() || FormationHandles.Contains(GroupID))
	{
		return false;
	}
	
	FFormationHandle FormationHandle;
	FormationHandle.FormationDataAsset = DataAsset;
	FormationHandles.Add(GroupID, FormationHandle);
	return true;
}

bool UFormationSubsystem::DestroyGroup(const FName GroupID)
{
	if (!FormationHandles.Contains(GroupID))
	{
		return false;
	}
	
	for (const TWeakObjectPtr<UFormationComponent>& Unit : FormationHandles[GroupID].Units)
	{
		RemoveUnit(GroupID, Unit.Get());
	}
	
	return FormationHandles.Remove(GroupID) > 0;
}

bool UFormationSubsystem::AddUnit(const FName GroupID, UFormationComponent* FormationUnit)
{
	if (!IsValid(FormationUnit))
	{
		return false;
	}
	
	if (!FormationHandles.Contains(GroupID))
	{
		return false;
	}
	
	FFormationHandle& FormationHandle = FormationHandles[GroupID];
	bool bIsAlreadyInSet = false;
	FormationHandle.Units.Add(FormationUnit, &bIsAlreadyInSet);
	if (!bIsAlreadyInSet)
	{
		RemoveUnit(FormationUnit->GetFormationID(), FormationUnit);
		FormationUnit->HandleFormationJoined(GroupID);
		FormationHandle.OnUnitJoined.Broadcast(FormationUnit);
		return true;
	}
	return false;
}

bool UFormationSubsystem::RemoveUnit(const FName GroupID, UFormationComponent* FormationUnit)
{
	if (!IsValid(FormationUnit))
	{
		return false;
	}

	FFormationHandle* FormationHandle = FormationHandles.Find(GroupID);
	if (!FormationHandle)
	{
		return false;
	}
	
	if (FormationHandle->Units.Remove(FormationUnit))
	{
		FormationUnit->HandleFormationLeft(GroupID);
		FormationHandle->OnUnitJoined.Broadcast(FormationUnit);

		if (!FormationHandle->Units.Num()) 
		{
			FormationHandles.Remove(GroupID);
		}

		return true;
	}
	
	return false;
}

bool UFormationSubsystem::GetUnits(const FName GroupID, TSet<UFormationComponent*>& Units) const
{
	const FFormationHandle* FormationHandle = FormationHandles.Find(GroupID);
	if (!FormationHandle)
	{
		return false;
	}
	
	Units.Reset();
	Units.Reserve(FormationHandle->Units.Num());
	Algo::TransformIf(FormationHandle->Units, Units,
		/* Predicate  */ [](const TWeakObjectPtr<UFormationComponent>& W){ return W.IsValid(); },
		/* Transform  */ [](const TWeakObjectPtr<UFormationComponent>& W){ return W.Get(); });
	return true;
}

int UFormationSubsystem::GetUnitsCount(const FName GroupID) const
{
	const FFormationHandle* FormationHandle = FormationHandles.Find(GroupID);
	return FormationHandle ? FormationHandle->Units.Num() : INDEX_NONE;
}

bool UFormationSubsystem::StopMovement(const FName GroupID) const
{
	const FFormationHandle* FormationHandle = FormationHandles.Find(GroupID);
	if (!FormationHandle)
	{
		return false;
	}

	for (const TWeakObjectPtr<UFormationComponent>& Unit : FormationHandle->Units)
	{
		if (Unit.IsValid())
		{
			Unit->StopMovement();
		}
	}
	
	return true;
}

bool UFormationSubsystem::MoveFormation(const FName GroupID, const FVector& Location, const FVector& Direction) const
{
	const FFormationHandle* FormationHandle = FormationHandles.Find(GroupID);
	if (!FormationHandle)
	{
		return false;
	}

	if (!FormationHandle->FormationDataAsset)
	{
		return false;
	}

	const int Count = FormationHandle->Units.Num();
	TArray<FTransform> WorldTransforms;
	FormationHandle->FormationDataAsset->GetWorldTransforms(Count, Location, Direction, WorldTransforms);
	TArray<TWeakObjectPtr<UFormationComponent>> FormationComponents = FormationHandle->Units.Array();

	for (int Index = 0; Index < Count; Index++)
	{
		TWeakObjectPtr<UFormationComponent>& Unit = FormationComponents[Index];
		if (Unit.IsValid())
		{
			Unit->SetupTarget(WorldTransforms[Index]);
		}
	}

	return true;
}

bool UFormationSubsystem::SetFormationDataAsset(const FName GroupID, UFormationDataAsset* InFormationDataAsset)
{
	FFormationHandle* FormationHandle = FormationHandles.Find(GroupID);
	if (!FormationHandle)
	{
		return false;
	}
	FormationHandle->FormationDataAsset = InFormationDataAsset;
	return true;
}

UFormationDataAsset* UFormationSubsystem::GetFormationDataAsset(const FName GroupID) const
{
	const FFormationHandle* FormationHandle = FormationHandles.Find(GroupID);
	if (!FormationHandle)
	{
		return nullptr;
	}
	return FormationHandle->FormationDataAsset;
}

FVector UFormationSubsystem::GetFormationAverageLocation(const FName GroupID) const
{
	const FFormationHandle* FormationHandle = FormationHandles.Find(GroupID);
	if (!FormationHandle)
	{
		return FVector::ZeroVector;
	}
	
	TArray<AActor*> Actors;
	for (const TWeakObjectPtr<UFormationComponent>& Unit : FormationHandle->Units)
	{
		Actors.Add(Unit->GetPawn());
	}
	return UGameplayStatics::GetActorArrayAverageLocation(Actors);
}

AActor* UFormationSubsystem::GetFormationLead(const FName GroupID) const
{
	const FFormationHandle* FormationHandle = FormationHandles.Find(GroupID);
	if (!FormationHandle)
	{
		return nullptr;
	}

	float LeadDistance = FLT_MAX;
	const UFormationComponent* Lead = nullptr;
	for (const TWeakObjectPtr<UFormationComponent>& Unit : FormationHandle->Units)
	{
		if (const float Distance = Unit->GetDistanceToDestination();
			Distance < LeadDistance)
		{
			LeadDistance = Distance;
			Lead = Unit.Get();
		}
	}

	return Lead ? Lead->GetPawn() : nullptr;
}

FVector UFormationSubsystem::GetFormationLeadLocation(const FName GroupID) const
{
	const AActor* Lead = GetFormationLead(GroupID);
	return IsValid(Lead) ? Lead->GetActorLocation() : FVector::ZeroVector;
}

FFormationHandleEvent* UFormationSubsystem::OnUnitJoined(const FName GroupID)
{
	if (FFormationHandle* FormationHandle = FormationHandles.Find(GroupID))
	{
		return &FormationHandle->OnUnitJoined;
	}
	return nullptr;
}

FFormationHandleEvent* UFormationSubsystem::OnUnitLeft(const FName GroupID)
{
	if (FFormationHandle* FormationHandle = FormationHandles.Find(GroupID))
	{
		return &FormationHandle->OnUnitLeft;
	}
	return nullptr;
}
