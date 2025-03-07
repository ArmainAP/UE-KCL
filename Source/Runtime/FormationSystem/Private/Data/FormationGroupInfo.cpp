// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Data/FormationGroupInfo.h"

#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/FormationComponent.h"
#include "Data/FormationDataAsset.h"

void UFormationGroupInfo::Setup(const TArray<UFormationComponent*>& InUnits)
{
	for (UFormationComponent* Unit : InUnits)
	{
		AddController(Unit);
	}
}

void UFormationGroupInfo::AddController(UFormationComponent* Unit)
{
	if (!IsValid(Unit))
	{
		return;
	}
	
	AActor* OwningActor = Cast<AActor>(Unit->GetOwner());
	if (!IsValid(OwningActor))
	{
		return;
	}
	
	OwningActor->OnDestroyed.AddUniqueDynamic(this, &UFormationGroupInfo::OnOwnerDestroyed);
	Unit->OnReached.AddUniqueDynamic(this, &UFormationGroupInfo::OnUnitReached);
	Units.Add(Unit);
}

void UFormationGroupInfo::RemoveController(UFormationComponent* Unit)
{
	if (!IsValid(Unit))
	{
		return;
	}

	if (AActor* OwningActor = Cast<AActor>(Unit->GetOwner()))
	{
		OwningActor->OnDestroyed.RemoveDynamic(this, &UFormationGroupInfo::OnOwnerDestroyed);
	}

	Unit->StopMovement();
	Units.Remove(Unit);
}

void UFormationGroupInfo::ChangeFormation(UFormationDataAsset* InFormationDataAsset)
{
	FormationDataAsset = InFormationDataAsset;
}

bool UFormationGroupInfo::HaveAllUnitsReached()
{
	for (UFormationComponent* Unit : Units)
	{
		if (!Unit->HasReached())
		{
			return false;
		}
	}
	return true;
}

void UFormationGroupInfo::StopMovement()
{
	for (UFormationComponent* Unit : Units)
	{
		Unit->StopMovement();
	}
}

void UFormationGroupInfo::MoveFormation(const FVector Location, const FVector Direction)
{
	if (!IsValid(FormationDataAsset))
	{
		return;
	}

	TArray<FTransform> WorldTransforms;
	FormationDataAsset->GetWorldTransforms(Units, Location, Direction, WorldTransforms);

	for (int Index = 0; Index < Units.Num(); Index++)
	{
		const FTransform& Transform = WorldTransforms[Index];
		const FVector WorldLocation = Transform.GetLocation();
		const FRotator Rotation = FRotator(Transform.GetRotation() * Direction.ToOrientationQuat());
		Units[Index]->SetupFormation(WorldLocation, Rotation);	
	}
}

void UFormationGroupInfo::OnUnitReached(UFormationComponent* Unit)
{
	if (HaveAllUnitsReached())
	{
		OnAllUnitsReached.Broadcast();
	}
}

void UFormationGroupInfo::OnOwnerDestroyed(AActor* DestroyedActor)
{
	const AAIController* AIC = UAIBlueprintHelperLibrary::GetAIController(DestroyedActor);
	if (!IsValid(AIC))
	{
		return;
	}

	RemoveController(AIC->GetComponentByClass<UFormationComponent>());
}
