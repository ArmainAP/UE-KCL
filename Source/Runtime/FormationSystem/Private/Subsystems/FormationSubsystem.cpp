// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Subsystems/FormationSubsystem.h"

#include "Components/FormationComponent.h"
#include "Kismet/GameplayStatics.h"

UFormationSubsystem* UFormationSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) { return nullptr; }
	const UWorld* World = WorldContextObject->GetWorld();
	return World ? World->GetSubsystem<UFormationSubsystem>() : nullptr; 
}

bool UFormationSubsystem::CreateGroup(const FName GroupID, UFormationDataAsset* DataAsset, AActor* FormationOwner)
{
	if (GroupID.IsNone() || FormationHandles.Contains(GroupID))
	{
		return false;
	}
	
	FFormationHandle FormationHandle;
	FormationHandle.FormationDataAsset = DataAsset ? DataAsset : GetMutableDefault<UFormationDataAsset>();
	FormationHandle.FormationOwner = FormationOwner;
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
		OnUnitJoined.Broadcast(GroupID, FormationUnit);
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
		OnUnitLeft.Broadcast(GroupID, FormationUnit);
		return true;
	}
	
	return false;
}

template <typename OutT>
bool UFormationSubsystem::GetUnits(const FName GroupID, OutT&& Output) const
{
	const FFormationHandle* FormationHandle = FormationHandles.Find(GroupID);
	if (!FormationHandle)
	{
		return false;
	}
	
	Output.Reset();
	Output.Reserve(FormationHandle->Units.Num());
	Algo::TransformIf(FormationHandle->Units, Output,
		/* Predicate  */ [](const TWeakObjectPtr<UFormationComponent>& W){ return W.IsValid(); },
		/* Transform  */ [](const TWeakObjectPtr<UFormationComponent>& W){ return W.Get(); });
	return true;
}


bool UFormationSubsystem::GetUnitsSet(const FName GroupID, TSet<UFormationComponent*>& Units) const
{
	return GetUnits(GroupID, Units);
}

bool UFormationSubsystem::GetUnitsArray(const FName GroupID, TArray<UFormationComponent*>& Units) const
{
	return GetUnits(GroupID, Units);
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
			Unit->SetMovementState(EMovementState::Stopped);
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

	const UFormationDataAsset* FormationDataAsset = FormationHandle->FormationDataAsset ? FormationHandle->FormationDataAsset : GetDefault<UFormationDataAsset>();
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

bool UFormationSubsystem::ForEachUnit(const FName GroupID, FFormationUnitCallable Callable) const
{
	const FFormationHandle* Handle = FormationHandles.Find(GroupID);
	if (!Handle) { return false; }
	for (const TWeakObjectPtr<UFormationComponent>& UnitPtr : Handle->Units)
	{
		if (UFormationComponent* Unit = UnitPtr.Get())
		{
			Callable(Unit);
		}
	}
	return true;
}

bool UFormationSubsystem::ForEachUnitBP(const FName GroupID, const FFormationUnitDynDelegate& BPDelegate) const
{
	if (BPDelegate.IsBound())
	{
		return ForEachUnit(GroupID, [&BPDelegate](UFormationComponent* Unit)
		{
			BPDelegate.Execute(Unit);
		});
	}
	return false;
}

AActor* UFormationSubsystem::GetFormationOwner(const FName GroupID)
{
	const FFormationHandle* FormationHandle = FormationHandles.Find(GroupID);
	return FormationHandle ? FormationHandle->FormationOwner.Get() : nullptr;
}
