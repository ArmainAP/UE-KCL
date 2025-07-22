// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Objects/FormationContext.h"
#include "Components/FormationComponent.h"
#include "Data/FormationDataAssets/FormationDataAsset.h"

bool UFormationContext::AddUnit(UFormationComponent* FormationUnit)
{
	if (!IsValid(FormationUnit))
	{
		return false;
	}
	
	bool bIsAlreadyInSet = false;
	Units.Add(FormationUnit, &bIsAlreadyInSet);
	if (!bIsAlreadyInSet)
	{
		FormationUnit->LeaveFormation();
		FormationUnit->HandleFormationJoined(this);
		OnUnitJoined.Broadcast(this, FormationUnit);
		return true;
	}
	
	return false;
}

bool UFormationContext::RemoveUnit(UFormationComponent* FormationUnit)
{
	if (!IsValid(FormationUnit))
	{
		return false;
	}
	
	if (Units.Remove(FormationUnit))
	{
		FormationUnit->HandleFormationLeft(this);
		OnUnitLeft.Broadcast(this, FormationUnit);
		return true;
	}
	
	return false;
}

void UFormationContext::ForEachUnit(const FFormationUnitCallable& Callable) const
{
	for (auto It = Units.CreateIterator(); It; ++It)
	{
		if (UFormationComponent* Unit = It->Get())
		{
			Callable(Unit);
		}
		else
		{
			It.RemoveCurrent();
		}
	}
	for (const TWeakObjectPtr<UFormationComponent>& UnitPtr : Units)
	{
		if (UFormationComponent* Unit = UnitPtr.Get())
		{
			Callable(Unit);
		}
	}
}

void UFormationContext::ForEachUnitBP(const FFormationUnitDynDelegate& BPDelegate) const
{
	if (BPDelegate.IsBound())
	{
		ForEachUnit([&BPDelegate](UFormationComponent* Unit)
		{
			BPDelegate.Execute(Unit);
		});
	}
}

int UFormationContext::CullInvalidUnits() const
{
	int CullCount = 0;
	for (auto It = Units.CreateIterator(); It; ++It)
	{
		if (!It->IsValid())
		{
			It.RemoveCurrent();
			CullCount++;
		}
	}
	return CullCount;
}

void UFormationContext::RequestStop() const
{
	ForEachUnit([&](UFormationComponent* Unit)
	{
		Unit->SetMovementState(EMovementState::Stopped);
	});
}

void UFormationContext::RequestMove(const FVector& Location, const FVector& Direction) const
{
	const UFormationDataAsset* UsedFormationDataAsset = FormationDataAsset ? FormationDataAsset : GetDefault<UFormationDataAsset>();
	const int Count = Units.Num();
	TArray<FTransform> WorldTransforms;
	UsedFormationDataAsset->GetWorldTransforms(Count, Location, Direction, WorldTransforms);
	TArray<TWeakObjectPtr<UFormationComponent>> FormationComponents = Units.Array();

	for (int Index = 0; Index < Count; Index++)
	{
		TWeakObjectPtr<UFormationComponent>& Unit = FormationComponents[Index];
		if (Unit.IsValid())
		{
			Unit->SetupTarget(WorldTransforms[Index]);
		}
	}
}

FVector UFormationContext::GetFormationAverageLocation() const
{
	FVector LocationSum = FVector::ZeroVector;
	int32 ActorCount = 0;
	
	ForEachUnit([&](const UFormationComponent* Unit)
	{
		if (const AActor* Actor = Unit->GetOwner())
		{
			LocationSum += Actor->GetActorLocation();
			ActorCount++;
		}
	});

	return ActorCount > 0 ? (LocationSum / ActorCount) : FVector::ZeroVector;
}

AActor* UFormationContext::GetFormationLead() const
{
	float LeadDistance = FLT_MAX;
	const UFormationComponent* Lead = nullptr;

	ForEachUnit([&](const UFormationComponent* Unit)
	{
		if (const float Distance = Unit->GetDistanceToDestination();
			Distance < LeadDistance)
		{
			LeadDistance = Distance;
			Lead = Unit;
		}
	});
	
	return Lead ? Lead->GetPawn() : nullptr;
}

template <typename OutT>
void UFormationContext::GetUnits(OutT&& Output) const
{
	Output.Reset();
	Output.Reserve(Units.Num());
	auto Predicate = [](const TWeakObjectPtr<UFormationComponent>& W){ return W.IsValid(); };
	auto Transform = [](const TWeakObjectPtr<UFormationComponent>& W){ return W.Get(); };
	Algo::TransformIf(Units, Output, Predicate, Transform);
}


void UFormationContext::GetUnitsSet(TSet<UFormationComponent*>& OutUnits) const
{
	GetUnits(OutUnits);
}

void UFormationContext::GetUnitsArray(TArray<UFormationComponent*>& OutUnits) const
{
	GetUnits(OutUnits);
}

bool UFormationContext::IsFull() const
{
	CullInvalidUnits();
	const int MaxUnits = FormationDataAsset->GetFormationLimit();
	if (MaxUnits < 0)
	{
		return false;
	}
	return Units.Num() >= MaxUnits;
}
