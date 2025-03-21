// Copyright to Kat Code Labs, SRL. All Rights Reserved.


#include "Components/FormationComponent.h"

#include "Logging.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Objects/FormationGroupInfo.h"

UFormationComponent::UFormationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UFormationComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<APawn>(GetOwner());
	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogFormationSystem, Error, TEXT("%s - Owner is not a Pawn!"), StringCast<TCHAR>(__FUNCTION__).Get());
		Deactivate();
	}
}

// Called every frame
void UFormationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (Execute_GetDistanceToDestination(this) > DestinationDistanceThreshold)
	{
		PerformDistanceToGroupCheck();
	}
	else
	{
		StopMovement_Implementation();
		OnReached.Broadcast(this);
		SetHasFallenBehind(false);
	}
}

void UFormationComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (GroupInfo)
	{
		GroupInfo->RemoveUnit(this);
	}
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UFormationComponent::SetupTarget_Implementation(const FTransform& InTransform)
{
	if (!UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), InTransform.GetLocation(), TargetLocation, nullptr, nullptr))
	{
		return;
	}
	TargetTransform = InTransform;
	bReached = false;
	OnMove.Broadcast(this);
	SetComponentTickEnabled(true);
}

void UFormationComponent::StopMovement_Implementation()
{
	bReached = true;
	OnStopped.Broadcast(this);
	SetComponentTickEnabled(false);
}

bool UFormationComponent::HasReached_Implementation()
{
	return bReached;
}

FTransform UFormationComponent::GetTransform_Implementation() const
{
	if (const AActor* Actor = Cast<AActor>(GetOwner()))
	{
		return Actor->GetActorTransform();
	}
	return FTransform();
}

void UFormationComponent::HandleFormationLeft_Implementation(UFormationGroupInfo* OldFormation)
{
	if (GroupInfo == nullptr)
	{
		return;
	}
	GroupInfo = nullptr;
	Execute_StopMovement(this);
	OnLeftGroup.Broadcast(OldFormation);
}

void UFormationComponent::HandleFormationJoined_Implementation(UFormationGroupInfo* NewFormation)
{
	if (GroupInfo == NewFormation)
	{
		return;
	}
	GroupInfo = NewFormation;
	OnJoinedGroup.Broadcast(NewFormation);

	if (GroupInfo->GetUnitsCount() > 1)
	{
		SetHasFallenBehind(GetDistanceTo(GroupInfo->GetFormationLeadLocation()) > CatchUpDistanceThreshold);	
	}
}

AActor* UFormationComponent::GetActor_Implementation() const
{
	return OwnerPawn;
}

float UFormationComponent::GetDistanceToDestination_Implementation() const
{
	return GetDistanceTo(TargetLocation);
}

bool UFormationComponent::ChangeFormationGroup(UFormationGroupInfo* NewFormation)
{
	bool bSuccess = false;
	if (GroupInfo)
	{
		bSuccess = GroupInfo->RemoveUnit(this);
	}

	if (NewFormation)
	{
		bSuccess = NewFormation->AddUnit(this);
	}

	return bSuccess;
}

UFormationGroupInfo* UFormationComponent::GetFormationGroupInfo()
{
	if (IsValid(GroupInfo))
	{
		return GroupInfo;
	}
	
	UFormationGroupInfo* NewGroup = NewObject<UFormationGroupInfo>(this);
	NewGroup->SetFormationDataAsset(DefaultFormationDataAsset);
	NewGroup->AddUnit(this);
	return NewGroup;
}

FVector UFormationComponent::GetTargetLocation() const
{
	return TargetLocation;
}

void UFormationComponent::PerformDistanceToGroupCheck()
{
	if (!IsValid(GroupInfo))
	{
		return;
	}

	if (GroupInfo->GetUnitsCount() < 2)
	{
		return;
	}

	if (GroupInfo->GetFormationLead() == OwnerPawn)
	{
		return;
	}
	
	const float DistanceToGroup = GetDistanceTo(GroupInfo->GetFormationLeadLocation());
	if (DistanceToGroup > FallBehindDistanceThreshold)
	{
		SetHasFallenBehind(true);
	}
	else if (DistanceToGroup < CatchUpDistanceThreshold)
	{
		SetHasFallenBehind(false);
	}
}

float UFormationComponent::GetDistanceTo(const FVector& Location) const
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return FLT_MAX;
	}
	
	if (!IsValid(OwnerPawn))
	{
		return FLT_MAX;
	}

	FVector PawnLocation = OwnerPawn->GetNavAgentLocation();
	if (!UNavigationSystemV1::K2_ProjectPointToNavigation(World, PawnLocation, PawnLocation, nullptr, nullptr))
	{
		return FLT_MAX;
	}

	const UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(World, PawnLocation, TargetLocation);
	if (!IsValid(Path))
	{
		return FLT_MAX;
	}
	
	return Path->GetPathLength();
}

void UFormationComponent::SetHasFallenBehind(const bool NewHasFallenBehind)
{
	if (bHasFallenBehind == NewHasFallenBehind)
	{
		return;
	}

	bHasFallenBehind = NewHasFallenBehind;
	if (bHasFallenBehind)
	{
		OnFallBehind.Broadcast(this);
	}
	else
	{
		OnCatchUp.Broadcast(this);
	}
}